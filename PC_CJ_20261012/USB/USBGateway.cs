using System;
using LINMaster.LIN;

namespace LINMaster.USB
{
    // =========================================================================
    // USBGateway - Sunshine board CP2102N virtual-COM transport
    //
    // The board USB connector is connected to a CP2102N USB-UART bridge.
    // Reuse the proven LIN gateway byte framing so USB and LIN carry the same
    // frame ID, control byte, PEC, and 8-byte ADI LED command.
    // =========================================================================
    public sealed class USBGateway : ILINGateway
    {
        private const int USB_BAUD_RATE = 115200;
        private readonly LINGateway _serialGateway;

        public USBGateway()
        {
            _serialGateway = new LINGateway(false);
            _serialGateway.BaudRate = USB_BAUD_RATE;
        }

        public bool IsConnected => _serialGateway.IsConnected;
        public string PortName => _serialGateway.PortName;

        public event Action<LINFrame> FrameReceived
        {
            add { _serialGateway.FrameReceived += value; }
            remove { _serialGateway.FrameReceived -= value; }
        }

        public event Action<string> StatusMessage
        {
            add { _serialGateway.StatusMessage += value; }
            remove { _serialGateway.StatusMessage -= value; }
        }

        public event Action<string> ErrorMessage
        {
            add { _serialGateway.ErrorMessage += value; }
            remove { _serialGateway.ErrorMessage -= value; }
        }

        public event Action<byte[], string> RawPacket
        {
            add { _serialGateway.RawPacket += value; }
            remove { _serialGateway.RawPacket -= value; }
        }

        public bool Connect(string portName)
        {
            return _serialGateway.Connect(portName);
        }

        public void Disconnect()
        {
            _serialGateway.Disconnect();
        }

        public void Poll()
        {
            _serialGateway.Poll();
        }

        public bool SendPublisher(LINFrame frame)
        {
            return _serialGateway.SendPublisher(frame);
        }

        public bool SendSubscriberRequest(byte frameID, byte dataLen,
            LINChecksumType csType = LINChecksumType.Enhanced)
        {
            return _serialGateway.SendSubscriberRequest(frameID, dataLen, csType);
        }

        public void Dispose()
        {
            _serialGateway.Dispose();
        }
    }
}
