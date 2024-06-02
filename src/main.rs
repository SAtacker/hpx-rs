// #![allow(unused_imports, unused_variables, dead_code)]
use std::{
    borrow::Borrow,
    ffi::{c_char, c_double, c_int, CString},
    ops::Deref,
    time::{Duration, SystemTime},
};

use std::env;
use std::os::unix::ffi::OsStrExt;
use std::ptr;

#[cxx::bridge]
mod ffi {

    unsafe extern "C++" {

        include!("hpx_rs_defs.h");
        include!("stencil.h");

        fn fibonacci(u: u64) -> u64;
        fn fibonacci_hpx(u: u64) -> u64;
        unsafe fn copy_parallel(src: *const i32, src_end: *const i32, dest: *mut i32);

        fn hpx_init() -> i32;
        unsafe fn start(
            func: unsafe fn(i32, *mut *mut c_char) -> i32,
            argc: i32,
            argv: *mut *mut c_char,
        ) -> i32;
        fn stop() -> i32;

        type stepper;

        fn new_stepper() -> SharedPtr<stepper>;

        // type partition_data
        type amoolspace;
        fn do_work(self: &stepper, a: usize, b: usize, c: usize) -> SharedPtr<amoolspace>;

        type partition;
        type space;
        fn get_space_from_amool_space(a: SharedPtr<amoolspace>) -> SharedPtr<space>;

        fn amool();
        fn rust_wait_all_space(a: SharedPtr<space>);
        fn rust_print_space(a: SharedPtr<space>);

    }
}

fn print_type_of<T>(_: &T) {
    println!("{}", std::any::type_name::<T>())
}

fn your_main() -> i32 {
    let fib_hpx = ffi::fibonacci_hpx(10);
    println!("fib hpx(10) = {:?}", fib_hpx);
    return 0;
}

fn rust_main(a: i32, b: *mut *mut c_char) -> i32 {
    stencil_main();
    ffi::amool();
    return ffi::stop();
}

fn stencil_main() -> i32 {
    let now = SystemTime::now();
    let step = ffi::new_stepper();
    let res = (step).as_ref().unwrap().do_work(10, 10, 45);
    let res2 = ffi::get_space_from_amool_space(res);
    let rees2_c = res2.clone();
    ffi::rust_wait_all_space(res2);
    let time_taken = now.elapsed().unwrap();
    ffi::rust_print_space(rees2_c);
    println!("rust elapsed = {:?}", time_taken.as_nanos());
    return 0;
}

#[no_mangle]
pub extern "C" fn hpx_main_rust(_argc: i32, _argv: *mut *mut i8) -> i32 {
    let src = vec![1, 2, 3, 4, 5];
    let mut dest = vec![0; src.len()];

    unsafe {
        ffi::copy_parallel(src.as_ptr(), src.as_ptr().add(src.len()), dest.as_mut_ptr());
    }

    println!("Destination: {:?}", dest);

    0
}

fn main() {
    // Convert from OsString to nul-terminated CString, truncating each argument
    // at the first inner nul byte if present.
    let args: Vec<CString> = env::args_os()
        .map(|os_str| {
            let bytes = os_str.as_bytes();
            CString::new(bytes).unwrap_or_else(|nul_error| {
                let nul_position = nul_error.nul_position();
                let mut bytes = nul_error.into_vec();
                bytes.truncate(nul_position);
                CString::new(bytes).unwrap()
            })
        })
        .collect();

    // Convert from Vec<CString> of owned strings to Vec<*mut c_char> of
    // borrowed string pointers.
    //
    // Once extern type stabilizes (https://github.com/rust-lang/rust/issues/43467)
    // and https://internals.rust-lang.org/t/pre-rfc-make-cstr-a-thin-pointer/6258
    // is implemented, and CStr pointers become thin, we can sidestep this step
    // by accumulating the args as Vec<Box<CStr>> up front, then simply casting
    // from *mut [Box<CStr>] to *mut [*mut CStr] to *mut *mut c_char.
    let argc = args.len();
    let mut argv: Vec<*mut c_char> = Vec::with_capacity(argc + 1);
    for arg in &args {
        argv.push(arg.as_ptr() as *mut c_char);
    }
    argv.push(ptr::null_mut()); // Nul terminator.

    let fib = ffi::fibonacci(10);
    println!("fib (non-hpx) (10) = {:?}", fib);

    ffi::hpx_init();

    let mut _a = 0;
    unsafe {
        _a = ffi::start(
            rust_main as fn(i32, *mut *mut c_char) -> i32,
            argc as i32,
            argv.as_mut_ptr(),
        );
    }
    println!("_a: {}", _a);
    // let _c = your_main();
    // let _b = ffi::stop();
}
