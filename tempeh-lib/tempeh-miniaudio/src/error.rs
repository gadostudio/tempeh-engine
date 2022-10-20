use core::ffi::c_int;

use num_derive::{FromPrimitive, ToPrimitive};
use num_traits::{FromPrimitive, ToPrimitive};
use tempeh_miniaudio_sys::ma_result_MA_SUCCESS;

#[repr(i32)]
#[derive(FromPrimitive, ToPrimitive, Debug)]
pub enum Error {
    UnknownError = -1,
    InvalidArgs = -2,
    InvalidOperation = -3,
    OutOfMemory = -4,
    OutOfRange = -5,
    AccessDenied = -6,
    DoesNotExist = -7,
    AlreadyExists = -8,
    TooManyOpenFiles = -9,
    InvalidFile = -10,
    TooBig = -11,
    PathTooLong = -12,
    NameTooLong = -13,
    NotDirectory = -14,
    IsDirectory = -15,
    DirectoryNotEmpty = -16,
    AtEnd = -17,
    NoSpace = -18,
    Busy = -19,
    IoError = -20,
    Interrupt = -21,
    UNAVAILABLE = -22,
    AlreadyInUse = -23,
    BadAddress = -24,
    BadSeek = -25,
    BadPipe = -26,
    Deadlock = -27,
    TooManyLinks = -28,
    NotImplemented = -29,
    NoMessage = -30,
    BadMessage = -31,
    NoDataAvailable = -32,
    InvalidData = -33,
    Timeout = -34,
    NoNetwork = -35,
    NotUnique = -36,
    NotSocket = -37,
    NoAddress = -38,
    BadProtocol = -39,
    ProtocolUnavailable = -40,
    ProtocolNotSupported = -41,
    ProtocolFamilyNotSupported = -42,
    AddressFamilyNotSupported = -43,
    SocketNotSupported = -44,
    ConnectionReset = -45,
    AlreadyConnected = -46,
    NotConnected = -47,
    ConnectionRefused = -48,
    NoHost = -49,
    InProgress = -50,
    Cancelled = -51,
    MemoryAlreadyMapped = -52,
    FormatNotSupported = -100,
    DeviceTypeNotSupported = -101,
    ShareModeNotSupported = -102,
    NoBackend = -103,
    NoDevice = -104,
    ApiNotFound = -105,
    InvalidDeviceConfig = -106,
    Loop = -107,
    DeviceNotInitialized = -200,
    DeviceAlreadyInitialized = -201,
    DeviceNotStarted = -202,
    DeviceNotStopped = -203,
    FailedToOpenBackendDevice = -301,
    FailedToStartBackendDevice = -302,
    FailedToStopBackendDevice = -303,
}

impl Error {
    pub(crate) fn from(num: c_int) -> Result<(), Self> {
        if num == ma_result_MA_SUCCESS {
            Ok(())
        } else {
            Err(unsafe { Error::from_i32(num).unwrap_unchecked() })
        }
    }
}
