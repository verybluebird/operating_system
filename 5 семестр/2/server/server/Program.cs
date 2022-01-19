using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace server
{
    class Program
    {
        static void Main(string[] args)
        {
            const int buff_s = 1000;
            // Создаем сокет
            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp); 

            try
            {
                Int32 port = 2011; // порт сервера
                string adr1 = "0.0.0.0";//;
                IPAddress localhost = IPAddress.Parse(adr1);
                IPEndPoint address = new IPEndPoint(localhost, port);

                server.Bind(address);

                Byte[] bytes = new Byte[buff_s];
                String data;

                while (true)
                {
                    Console.Write("Waiting for a connection... ");
                    server.Listen(1);

                    Socket new_s = server.Accept();
                    Console.WriteLine("Connected!");

                    new_s.Receive(bytes);

                    int i = 0;
                    for (; bytes[i] != 0;)
                        i++;


                    if (i > 0)
                    {
                        IPEndPoint endPoint = new_s.LocalEndPoint as IPEndPoint;
                        string localIP = endPoint.Address.ToString();
                        data = Encoding.ASCII.GetString(bytes, 0, i);
                        Console.WriteLine("Received: {0}", data);

                        string answer_message = data + " "+ localIP + " " + port;
                        
                        byte[] msg = Encoding.ASCII.GetBytes(answer_message);

                        new_s.Send(msg, msg.Length, SocketFlags.None);
                        Console.WriteLine("Sent: {0}", answer_message);
                    }
                    new_s.Close();

                }
            }
            catch (SocketException expt)
            {
                Console.WriteLine("SocketException: {0}", expt);
            }
            finally
            {
                server.Dispose();
            }
        }
    }
}
