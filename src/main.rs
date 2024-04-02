#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {

        include!("hpx_rs_defs.h");

        fn fibonacci(u: u64) -> u64;
        fn fibonacci_hpx(u: u64) -> u64;

        fn init() -> i32;
        fn finalize() -> i32;


    }
}

fn main() {
    let fib = ffi::fibonacci(10);
    println!("fib (non-hpx) (10) = {:?}", fib);

    let _a = ffi::init();
    // let fib_hpx = ffi::fibonacci_hpx(10);
    // println!("fib hpx(10) = {:?}", fib_hpx);
    // let _b = ffi::finalize();
}
