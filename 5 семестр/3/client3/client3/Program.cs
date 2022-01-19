using System;
using System.Text;
using System.Net.Sockets;

namespace client3
{
    class Program
    {
         static void recvMessage(Socket client)
        {
            try
            {
                Byte[] data = new Byte[1000];

                client.Receive(data, SocketFlags.None);
                int i = 0;
                for (; data[i] != 0;)
                    i++;


                String responseData = Encoding.ASCII.GetString(data, 0, i);

                //печатаем то, что получили
                Console.WriteLine("Received: {0}", responseData);
            }
            catch (ArgumentNullException expt)
            {
                Console.WriteLine("ArgumentNullException: {0}", expt);
            }
            catch (SocketException expt)
            {
                Console.WriteLine("SocketException: {0}", expt);
            }


        }
        static void Main(string[] args)
        {
            try
            {

                Console.WriteLine("Enter ip of server:");
                string localhost = Console.ReadLine();
                Console.WriteLine("Enter port of server: ");
                Int32 port = Convert.ToInt32(Console.ReadLine());
                Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                client.Connect(localhost, port);
                Console.WriteLine("Enter name:");
                string message = Console.ReadLine(); // строка, которую пошлем серверу
                Byte[] data = System.Text.Encoding.ASCII.GetBytes(message);
                client.Send(data, data.Length, SocketFlags.None);


                while (true)
                {
                    Console.WriteLine("Enter message to server:");
                    message = Console.ReadLine(); // строка, которую пошлем серверу
                    data = System.Text.Encoding.ASCII.GetBytes(message);


                    Console.WriteLine("Sent: {0}", message);//печатаем то, что отправили

                    

                    client.Send(data, data.Length, SocketFlags.None);
                    // буффер для приема сообщений
                    data = new Byte[1000];
                    System.Threading.Thread th_add = new System.Threading.Thread(delegate () { recvMessage(client); });
                    th_add.Start();
                    // строка для приема сообщений сервера
                    /*client.Receive(data, SocketFlags.None);
                    int i = 0;
                    for (; data[i] != 0;)
                        i++;


                    String responseData = Encoding.ASCII.GetString(data, 0, i);

                    //печатаем то, что получили
                    Console.WriteLine("Received: {0}", responseData);*/
                }

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