use std::future::*;
use std::task::*;

const PENDING_SLEEP_MS: u64 = 10;

unsafe fn rwclone(_p: *const ()) -> RawWaker {
    make_raw_waker()
}
unsafe fn rwwake(_p: *const ()) {}
unsafe fn rwwakebyref(_p: *const ()) {}
unsafe fn rwdrop(_p: *const ()) {}

static VTABLE: RawWakerVTable = RawWakerVTable::new(rwclone, rwwake, rwwakebyref, rwdrop);

fn make_raw_waker() -> RawWaker {
    static DATA: () = ();
    RawWaker::new(&DATA, &VTABLE)
}

pub trait BlockingFuture: Future + Sized {
    fn block(self) -> <Self as Future>::Output {
        let mut boxed = Box::pin(self);
        let waker = unsafe { Waker::from_raw(make_raw_waker()) };
        let mut ctx = Context::from_waker(&waker);
        loop {
            match boxed.as_mut().poll(&mut ctx) {
                Poll::Ready(x) => {
                    return x;
                }
                Poll::Pending => {
                    std::thread::sleep(std::time::Duration::from_millis(PENDING_SLEEP_MS))
                }
            }
        }
    }
}

impl<F: Future + Sized> BlockingFuture for F {}
