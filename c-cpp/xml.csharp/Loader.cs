using System;

namespace Xml.Example
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Loader
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			XmlHandler file = new XmlHandler();
            
            bool loop = true;
            string iput;
            int choice;
            while(loop) 
            {
                Console.WriteLine("========================================");
                Console.WriteLine("1: Add Record");
                Console.WriteLine("2: Remove Record");
                Console.WriteLine("3: Print XML Document");
                Console.WriteLine("4: Exit");
                
                iput = Console.ReadLine();
                try 
                {
                    choice = Int32.Parse(iput);
                }
                catch 
                {
                    Console.WriteLine("Pick 1 - 4");
                    continue;
                }
                Console.WriteLine("========================================");
                switch(choice) 
                {
                    case 1:
                        Console.Write("Title : ");
                        string title = Console.ReadLine();
                        Console.Write("Sender : ");
                        string snder = Console.ReadLine();
                        Console.Write("Message : ");
                        string mess = Console.ReadLine();
                        file.AddRecord(title, snder, mess);
                        break;
                    case 2:
                        Console.Write("Title to Delete : ");
                        file.RemoveRecord(Console.ReadLine());
                        break;
                    case 3:
                        file.PrintDocument();
                        break;
                    case 4:
                        return;
                }
            }
		}
	}
}
