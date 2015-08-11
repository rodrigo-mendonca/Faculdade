using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace SOFM
{
    public delegate void EndEpochEventHandler(object sender, EndEpochEventArgs e);
    public delegate void EndIterationEventHandler(object sender, EventArgs e);

    public class NeuralNetwork
    {
        private Neuron[,] outputLayer;

        public Neuron[,] OutputLayer
        {
            get { return outputLayer; }
            set { outputLayer = value; }
        }
        private int inputLayerDimension;
        private int outputLayerDimension;
        private int numberOfPatterns;
        private List<List<double>> patterns;
        private List<string> classes;
        private SortedList<string, int> existentClasses;
        private List<System.Drawing.Color> usedColors;
        private bool normalize;
        private int numberOfIterations;
        private int currentIteration;

        private Functions function;
        private double epsilon;
        private double currentEpsilon;

        private double CalculateNormOfVectors(List<double> vector1, List<double> vector2)
        {
            double value = 0;
            for(int i=0; i<vector1.Count; i++)
                value += Math.Pow((vector1[i] - vector2[i]), 2);
            value = Math.Sqrt(value);
            return value;
        }

        private void NormalizeInputPattern(List<double> pattern)
        {
            // Что-то тут не то! Проверить!
            double nn = 0;
            for (int i = 0; i < inputLayerDimension; i++)
            {
                nn += (pattern[i] * pattern[i]);
            }
            nn = Math.Sqrt(nn);
            for (int i = 0; i < inputLayerDimension; i++)
            {
                pattern[i] /= nn;
            }
        }

        private void StartEpoch(List<double> pattern)
        {
            Neuron Winner = this.FindWinner(pattern);
            currentEpsilon = 0;
            for (int i = 0; i < outputLayerDimension; i++)
                for (int j = 0; j < outputLayerDimension; j++)
                {
                    currentEpsilon += outputLayer[i, j].ModifyWights(pattern, Winner.Coordinate, currentIteration, function);                   
                }
            currentIteration++;
            currentEpsilon = Math.Abs(currentEpsilon / (outputLayerDimension * outputLayerDimension));
            EndEpochEventArgs e = new EndEpochEventArgs();
            OnEndEpochEvent(e);
        }

        public bool Normalize
        {
            get { return normalize; }
            set { normalize = value; }
        }

        public List<List<double>> Patterns
        {
            get { return patterns; }
        }

        public List<string> Classes
        {
            get { return classes; }
        }

        public int InputLayerDimension
        {
            get { return inputLayerDimension; }
        }

        public int OutputLayerDimension
        {
            get { return outputLayerDimension; }
        }

        public double CurrentDelta
        {
            get { return currentEpsilon; }
        }

        public SortedList<string, int> ExistentClasses
        {
            get { return existentClasses; }
        }

        public List<System.Drawing.Color> UsedColors
        {
            get { return usedColors; }
        }

        private int NumberOfClasses()
        {
            existentClasses = new SortedList<string, int>();
            existentClasses.Add(classes[0], 1);
            int k = 0;
            int d = 2;
            for (int i = 1; i < classes.Count; i++)
            {
                k=0;
                for (int j = 0; j < existentClasses.Count; j++)
                    if (existentClasses.IndexOfKey(classes[i])!=-1) k++;
                if (k == 0)
                {
                    existentClasses.Add(classes[i],d);
                    d++;
                }
            }
            return existentClasses.Count;
        }

        public System.Drawing.Color[,] ColorSOFM()
        {
            System.Drawing.Color[,] colorMatrix = new System.Drawing.Color[outputLayerDimension, outputLayerDimension];
            int numOfClasses = NumberOfClasses();
            List<System.Drawing.Color> goodColors = new List<System.Drawing.Color>();
            goodColors.Add(System.Drawing.Color.Black);
            goodColors.Add(System.Drawing.Color.Red);
            goodColors.Add(System.Drawing.Color.Navy);
            goodColors.Add(System.Drawing.Color.Green);
            goodColors.Add(System.Drawing.Color.Yellow);            
            usedColors = new List<System.Drawing.Color>(numOfClasses);
            usedColors.Add(goodColors[0]);
            int k = 0;
            int randomColor = 0;
            Random r = new Random();
            while (usedColors.Count != numOfClasses)
            {
                k = 0;
                randomColor = r.Next(goodColors.Count);
                foreach (System.Drawing.Color cl in usedColors)
                    if (cl == goodColors[randomColor]) k++;
                if (k == 0) usedColors.Add(goodColors[randomColor]);
            }
            for (int i = 0; i < outputLayerDimension; i++)
                for (int j = 0; j < outputLayerDimension; j++)
                    colorMatrix[i, j] = System.Drawing.Color.FromKnownColor(System.Drawing.KnownColor.ButtonFace);

            for (int i = 0; i < patterns.Count; i++)
            {
               Neuron n = FindWinner(patterns[i]);
                colorMatrix[n.Coordinate.X,n.Coordinate.Y] = usedColors[existentClasses[classes[i]]-1];
            }
            return colorMatrix;
        }

        public NeuralNetwork(int m, int numberOfIterations, double epsilon, Functions f)
        {
            outputLayerDimension = m;
            currentIteration = 1;
            this.numberOfIterations = numberOfIterations;
            function = f;
            this.epsilon = epsilon;
            currentEpsilon = 100;
        }

        public Neuron FindWinner(List<double> pattern)
        {
            List<double> norms = new List<double>(outputLayerDimension * outputLayerDimension);
            double D = 0;
            Neuron Winner = outputLayer[0, 0];
            double min = CalculateNormOfVectors(pattern, outputLayer[0, 0].Weights);
            for (int i = 0; i < outputLayerDimension; i++)
                for (int j = 0; j < outputLayerDimension; j++)
                {
                    D = CalculateNormOfVectors(pattern, outputLayer[i, j].Weights);
                    if (D < min)
                    {
                        min = D;
                        Winner = outputLayer[i, j];
                    }
                }
            return Winner;
        }

        public void StartLearning()
        {
            int iterations = 0;
            while (iterations<=numberOfIterations && currentEpsilon > epsilon)
            {
                List<List<double>> patternsToLearn = new List<List<double>>(numberOfPatterns);
                foreach (List<double> pArray in patterns)
                    patternsToLearn.Add(pArray);
                Random randomPattern = new Random();
                List<double> pattern = new List<double>(inputLayerDimension);
                for (int i = 0; i < numberOfPatterns; i++)
                {
                    pattern = patternsToLearn[randomPattern.Next(numberOfPatterns - i)];

                    StartEpoch(pattern);

                    patternsToLearn.Remove(pattern);
                }
                iterations++;
                OnEndIterationEvent(new EventArgs());
            }
        }

        public void ReadDataFromFile(string inputDataFileName)
        {
            StreamReader sr = new StreamReader(inputDataFileName);
            string line = sr.ReadLine();
            int k = 0;
            for (int i = 0; i < line.Length; i++)
            {
                if (line[i] == ' ') k++;
            }
  
            inputLayerDimension = k;
            int sigma0 = outputLayerDimension;
            
            outputLayer = new Neuron[outputLayerDimension, outputLayerDimension];
            Random r = new Random();
            for (int i = 0; i < outputLayerDimension; i++)
                for (int j = 0; j < outputLayerDimension; j++)
                {
                    outputLayer[i, j] = new Neuron(i, j, sigma0);
                    outputLayer[i, j].Weights = new List<double>(inputLayerDimension);
                    for (k = 0; k < inputLayerDimension; k++)
                    {
                        outputLayer[i, j].Weights.Add(r.NextDouble());
                    }
                }

            k = 0;
            while (line != null)
            {
                line = sr.ReadLine();
                k++;
            }
            patterns = new List<List<double>>(k);
            classes = new List<string>(k);
            numberOfPatterns = k;

            List<double> pattern;

            sr = new StreamReader(inputDataFileName);
            line = sr.ReadLine();
           
            while (line != null)
            {
                int startPos = 0;
                int endPos = 0;
                int j = 0;
                pattern = new List<double>(inputLayerDimension);
                for (int ind = 0; ind < line.Length; ind++)
                {
                    if (line[ind] == ' ' && j != inputLayerDimension)
                    {
                        endPos = ind;
                        pattern.Add(Convert.ToDouble(line.Substring(startPos, endPos - startPos)));
                        startPos = ind + 1;
                        j++;
                    }
                    if (j > inputLayerDimension) throw new InvalidDataException("Wrong file format. Check input data file, and try again");
                }
                if (normalize) this.NormalizeInputPattern(pattern);
                patterns.Add(pattern);
                startPos = line.LastIndexOf(' ');
                classes.Add(line.Substring(startPos));
                line = sr.ReadLine();
            }
        }

        public event EndEpochEventHandler EndEpochEvent;
        public event EndIterationEventHandler EndIterationEvent;

        protected virtual void OnEndEpochEvent(EndEpochEventArgs e)
        {
            if (EndEpochEvent != null)
                EndEpochEvent(this, e);
        }

        protected virtual void OnEndIterationEvent(EventArgs e)
        {
            if (EndIterationEvent != null)
                EndIterationEvent(this, e);
        }
    }
}