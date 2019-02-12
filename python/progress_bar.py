
import time
import threading

def progressBar(progress, barWidth = 50):

    sys.stdout.write('[')
    pos = int(barWidth * progress)

    for i in xrange(barWidth):
        if i < pos:
            sys.stdout.write('=')
        elif i == pos:
            sys.stdout.write('>')
        else: sys.stdout.write(' ')

    sys.stdout.write('] {}%\r'.format(int(progress * 100)))
    sys.stdout.flush()


def testProgBar():
    for i in [0,5,10,20,50,75,100]:
        time.sleep(0.5)
        progressBar(float(i) / float(100))

    sys.stdout.write('\n')


class FileModifications():
    def __init__(self):
        self.inputDir = ''
        self.modTimes = dict()

    def initialize(self, inputDir, sourceExt, outExt):
        inputDirTemp = os.path.realpath(inputDir)
        sourceFiles = ['{}/{}'.format(inputDirTemp, x.replace(sourceExt, outExt))
        #sourceFiles = ['{}/{}'.format(inputDirTemp, x)
                       for x in os.listdir(inputDirTemp)
                       if re.search('{}$'.format(sourceExt), x)]

        for file in sourceFiles:
            try:
                time = os.path.getmtime(file)
            except OSError:
                self.modTimes[file] = 0
                print(file)
                continue
            self.modTimes[file] = time
        print('poop')