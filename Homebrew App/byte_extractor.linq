<Query Kind="Statements" />

var path = @"C:\Users\Jas\Documents\GitHub\LagTestHomebrew\120HzTone.raw";

var bytes = File.ReadAllBytes(path);

var byteString = bytes.Select(b => b.ToString()).Aggregate((one, two) => one + "," + two);
string.Format("char audioSignal[{0}] = {{{1}}};", bytes.Length, byteString).Dump();