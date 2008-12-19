using System;
using System.Xml;

namespace Xml.Example 
{
    public class XmlHandler 
    {
        private XmlDocument doc;
        public XmlHandler() 
        {
            doc = new XmlDocument();
            doc.LoadXml("<Records/>");
        }

        public void AddRecord(string t, string s, string m) 
        {
            XmlNode root = doc.DocumentElement;
            XmlElement rec = doc.CreateElement("Singleton");
            XmlElement snd = doc.CreateElement("Sender");
            XmlElement msg = doc.CreateElement("Message");
            XmlAttribute title = doc.CreateAttribute("Title");
            title.InnerText = t;
            snd.InnerText = s;
            msg.InnerText = m;
            rec.AppendChild(snd);
            rec.AppendChild(msg);
            rec.Attributes.Append(title);
            root.AppendChild(rec);
        }

        public void PrintDocument() 
        {
            doc.Save(Console.Out);
            Console.WriteLine();
        }

        public void RemoveRecord(string t) 
        {
            XmlNode root = doc.DocumentElement;
            XmlNode name;
            for(int i = 0; i < root.ChildNodes.Count; i++) 
            {
                name = root.ChildNodes[i].Attributes["Title"];
                if(name.InnerText == t) 
                {
                    root.RemoveChild(root.ChildNodes[i]);
                    break;
                }
            }
        }
    }
}