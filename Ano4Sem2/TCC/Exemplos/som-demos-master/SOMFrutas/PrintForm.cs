using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SOMFrutas
{
    public partial class PrintForm : Form
    {
        public Bitmap Antes;
        public Bitmap Depois;

        public PrintForm()
        {
            InitializeComponent();
        }

        private void PrintForm_Load(object sender, EventArgs e)
        {
            pictureBox1.Image = Antes;
            pictureBox2.Image = Depois;
        }
    }
}
