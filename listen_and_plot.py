import sys
import time


Usage=f"""
  %s data_file.txt channel
"""%str(sys.argv[0])

if len(sys.argv) < 3:
    print(Usage)
    exit()

datafile = str(sys.argv[1])
doall = bool(str(sys.argv[2]) == "1")
print(f'DO ALL? {doall}')

if datafile == '-h' or datafile == '--help':
    print(Usage)
    exit()


if __name__ == "__main__":

    print(f"Importing ROOT")
    import ROOT

    hist0 = ROOT.TH1F("h0","Streaming histo",5000,0,5000)
    hist1 = ROOT.TH1F("h1","Streaming histo",5000,0,5000)
    hist2 = ROOT.TH1F("h2","Streaming histo",5000,0,5000)
    hist3 = ROOT.TH1F("h3","Streaming histo",5000,0,5000)
    H23 = ROOT.TH2F("h23","Streaming histo",5000,0,5000,5000,0,5000)

    c = ROOT.TCanvas("c", "", 1200, 800)
    c.Divide(3,2)

    print('Running')
    with open(datafile, "r") as f:

    # vai alla fine del file
        if not doall:
            f.seek(0, 2)

        while True:

            line = f.readline()

            if not line:
                if not doall:
                    time.sleep(0.1)
                continue

            try:
                values = line.strip().split()
                value0 = float(values[0])
                value1 = float(values[1])
                value2 = float(values[2])
                value3 = float(values[3])
                hist0.Fill(value0)
                hist1.Fill(value1)
                hist2.Fill(value2)
                hist3.Fill(value3)
                H23.Fill(value2,value3)

                if not doall:
                    c.cd(1)
                    hist0.Draw()
                    c.cd(2)
                    hist1.Draw()
                    c.cd(3)
                    hist2.Draw()
                    c.cd(4)
                    hist3.Draw()
                    c.cd(5)
                    H23.Draw("colz")
                
                    c.Update()
            
            except ValueError:
                pass

    if doall:
        c.cd(1)
        hist0.Draw()
        c.cd(2)
        hist1.Draw()
        c.cd(3)
        hist2.Draw()
        c.cd(4)
        hist3.Draw()
        c.cd(5)
        H23.Draw("colz")
        c.Update()
