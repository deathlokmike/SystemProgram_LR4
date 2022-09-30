using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;

namespace Form1
{
    public partial class Form1 : Form
    {
        [DllImport("MFCLibrary.dll", CharSet = CharSet.Ansi)] private static extern void SendString(string str, int threadNumber);
        [DllImport("MFCLibrary.dll")] public static extern int SendInt(int n);
        [DllImport("MFCLibrary.dll")] public static extern int GetInt();
        [DllImport("MFCLibrary.dll")] private static extern bool ConnectToServer();
        [DllImport("MFCLibrary.dll")] private static extern void Disconnect();
        public Form1()
        {
            InitializeComponent();
        }

        private void add_child(int num_thread)
        {
            if (treeView1.Nodes.Count == 0)
                treeView1.Nodes.Add("Главный поток");
            if (treeView1.Nodes.Count == 1)
            {
                if (treeView1.Nodes[0].Text == "Нет ответа сервера")
                {
                    treeView1.Nodes.Clear();
                    treeView1.Nodes.Add("Главный поток");
                }
                treeView1.Nodes.Add("Все потоки");
            }
                
            if (treeView1.Nodes.Count >= 2)
            {
                if (num_thread > treeView1.Nodes[1].Nodes.Count)
                {
                    int buf = num_thread - treeView1.Nodes[1].Nodes.Count;
                    for (int i = 0; i < buf; i++)
                    {
                        treeView1.Nodes[1].Nodes.Add("Поток " + (treeView1.Nodes[1].Nodes.Count + 1).ToString());
                    }
                }
                else
                    treeView1.Nodes[1].Nodes.Clear();
            }
        }

        private void treeView1_AfterCheck(object sender, TreeViewEventArgs e)
        {
            foreach (TreeNode child in e.Node.Nodes)
                child.Checked = e.Node.Checked;
        }
        
        private List<int> get_current_num() 
        {
            List<int> buff = new List<int>();
            if (treeView1.Nodes.Count > 0)
            {
                if (treeView1.Nodes[0].Checked)
                    buff.Add(-2);
                if (treeView1.Nodes.Count > 1)
                {
                    if (treeView1.Nodes[1].Checked)
                        buff.Add(-1);
                    else
                    {
                        int j = 0;
                        foreach (TreeNode child in treeView1.Nodes[1].Nodes)
                        {
                            if (child.Checked)
                                buff.Add(j);
                            j += 1;
                        }
                    }    
                }
            }
            return buff;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            if (ConnectToServer())
            {
                int maxThreads = Convert.ToInt32(textBox1.Text);
                SendInt(0);
                SendInt(maxThreads);
                add_child(GetInt());
                Disconnect();
            }
            else
            {
                treeView1.Nodes.Clear();
                treeView1.Nodes.Add("Нет ответа сервера");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (treeView1.Nodes.Count != 1)
            {
                ConnectToServer();
                SendInt(1);
                add_child(GetInt());
                Disconnect();
            }
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            if (ConnectToServer())
            {
                ConnectToServer();
                SendInt(20);
                add_child(GetInt());
                Disconnect();
            }
            else
            {
                treeView1.Nodes.Clear();
                treeView1.Nodes.Add("Нет ответа сервера");
            }
        }
        private void button3_Click(object sender, EventArgs e)
        {
            ConnectToServer();
            SendInt(2);
            string text = textBox2.Text;
            List<int> thread_nums = get_current_num();
            if (text.Length == 0 || thread_nums.Count == 0) return;
            foreach (int threadNumber in thread_nums)
            {
                SendString(text, threadNumber);
                Disconnect();
            }
        }
    }
}
