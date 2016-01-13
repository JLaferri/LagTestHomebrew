<Query Kind="Statements" />

var path = @"C:\Users\Jas\Documents\GitHub\LagTestHomebrew\120HzTone_44100Hz_32bitFloat.raw";

var bytes = File.ReadAllBytes(path);

Enumerable.Range(0, bytes.Length / 4).Select(i => BitConverter.ToSingle(bytes, i * 4)).Dump();
//var byteString = bytes.Select(b => b.ToString()).Aggregate((one, two) => one + "," + two);
//string.Format("char audioSignal[{0}] = {{{1}}};", bytes.Length, byteString).Dump();