using System;
using System.Text;
using System.Net.Sockets;

namespace client
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                Console.WriteLine("Enter message to server:");
                string message = Console.ReadLine(); // строка, которую пошлем серверу
                Console.WriteLine("Enter ip of server:");
                string localhost = Console.ReadLine();
                Console.WriteLine("Enter port of server: ");
                Int32 port = Convert.ToInt32(Console.ReadLine());
                Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);

                Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                Console.WriteLine("Sent: {0}", message);//печатаем то, что отправили

                client.Connect(localhost, port);

                client.Send(data, data.Length, SocketFlags.None);
                // буффер для приема сообщений
                data = new Byte[1000];
                // строка для приема сообщений сервера
                client.Receive(data, SocketFlags.None);
                int i = 0;
                for (; data[i] != 0;)
                    i++;

                String responseData = Encoding.ASCII.GetString(data, 0, i);

                //печатаем то, что получили
                Console.WriteLine("Received: {0}", responseData);

                // закрываем соединение
                client.Close();
            }
            catch (ArgumentNullException expt)
            {
                Console.WriteLine("ArgumentNullException: {0}", expt);
            }
            catch (SocketException expt)
            {
                Console.WriteLine("SocketException: {0}", expt);
            }

            Console.WriteLine("\n Press Enter to continue...");
            Console.Read();
        }
    }
}