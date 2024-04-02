#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {

        include!("hpx_rs_defs.h");

        fn fibonacci(u: u64) -> u64;
        fn fibonacci_hpx(u: u64) -> u64;

        fn start() -> i32;
        fn stop() -> i32;

    }
}

fn your_main() -> i32 {
    let fib_hpx = ffi::fibonacci_hpx(10);
    println!("fib hpx(10) = {:?}", fib_hpx);
    return 0;
}

fn main() {
    let fib = ffi::fibonacci(10);
    println!("fib (non-hpx) (10) = {:?}", fib);

    let _a = ffi::start();
    let _c = your_main();
    let _b = ffi::stop();
}
