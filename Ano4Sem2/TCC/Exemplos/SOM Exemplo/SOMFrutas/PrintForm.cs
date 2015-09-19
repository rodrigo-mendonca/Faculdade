using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MySOM;
using System.Threading;

namespace SOMFrutas
{
    public partial class PrintForm : Form
    {
        Kohonen oMap;
        public Bitmap Antes;
        public Bitmap Depois;
        int frame = 0;
        public PrintForm()
        {
            InitializeComponent();
        }

        private void PrintForm_Load(object sender, EventArgs e)
        {
            
        }

        private void cmdTreinar_Click(object sender, EventArgs e)
        {
            cmdTreinar.Enabled = false;
            oMap = new Kohonen(3, (int)NumTreinamento.Value);
            pictureBox1.Image = oMap.oAntes;
            pictureBox1.Refresh();
            oMap.Init("Food.txt",new EventHandler(Teste));
            pictureBox2.Image = oMap.oDepois;
            cmdTreinar.Enabled = true;
        }

        private void Teste(object sender, EventArgs e)
        {
            
            Bitmap pic = (Bitmap)sender;

            if (frame % 10000 == 0)
            {
                pictureBox2.Image = pic;
                pictureBox2.Refresh();
                Thread.Sleep(1 / 500);
                
            }
            frame++;
            
            //lblFrame.Text = frame.ToString();
        }
    }
}
