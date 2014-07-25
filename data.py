class Fixation(object):

    def __init__(self, fd):
        self.fields = ['time', 'start', 'end', 'hstx', 'hsty', 
                'gstx', 'gsty', 'sta', 'henx', 'heny', 'genx',
                'ena', 'havx', 'havy', 'gavx', 'gavy', 'ava', 
                'avel', 'pvel', 'svel', 'evel', 'supd_x', 'eupd_x',
                'supd_y', 'eupd_y', 'eye', 'buttons', 'message']
        self.sample_fields = ['px', 'py', 'hx', 'hy', 'pa', 'gx', 'gy', 
                'rx', 'ry', 'gxvel', 'gyvel', 'hxvel', 'hyvel', 'rxvel',
                'ryvel', 'fgxvel', 'fgyvel', 'fhxvel', 'fhyvel', 'frxvel', 
                'fryvel', 'hdata', 'input']
        self.time = fd.fe.time
        self.start = fd.fe.sttime
        self.end = fd.fe.entime
        self.hstx, self.hsty = fd.fe.hstx, fd.fe.hsty
        self.gstx, self.gsty = fd.fe.gstx, fd.fe.gsty
        self.sta = fd.fe.sta
        self.henx, self.heny = fd.fe.henx, fd.fe.heny
        self.genx, self.geny = fd.fe.genx, fd.fe.geny
        self.ena = fd.fe.ena
        self.havx, self.havy = fd.fe.havx, fd.fe.havy
        sellf.gavx, self.gavy = fd.fe.gavx, fd.fe.gavy
        self.ava, self.avel = fd.fe.ava, fd.fe.avel
        self.pvel, self.svel, self.evel = fd.fe.pvel, fd.fe.svel, fd.fe.evel
        self.supd_x, self.eupd_x = fd.fe.supd_x, fd.fe.eupd_x
        self.supd_y, self.eupd_y = fd.fe.supd_x, fd.fe.eupd_y
        self.eye, self.buttons, self.message = fd.fe.eye, fd.fe.buttons, fd.fe.message
        self.samples = dict((key, []) for key in self.sample_fields)

    def add_sample(self, fd):
        self.samples['time'].append(fd.fs.time)
        self.samples['px'].append(fd.fs.px)
        self.samples['py'].append(fd.fs.py)
        self.samples['hx'].append(fd.fs.hx)
        self.samples['hy'].append(fd.fs.hy)
        self.samples['gx'].append(fd.fs.gx)
        self.samples['gy'].append(fd.fs.gy)
        self.samples['pa'].append(fd.fs.pa)
        self.samples['rx'].append(fd.fs.rx)
        self.samples['ry'].append(fd.fs.ry)
        self.samples['gxvel'].append(fd.fs.gxvel)
        self.samples['gyvel'].append(fd.fs.gyvel)
        self.samples['hxvel'].append(fd.fs.hxvel)
        self.samples['hyvel'].append(fd.fs.hyvel)
        self.samples['rxvel'].append(fd.fs.rxvel)
        self.samples['ryvel'].append(fd.fs.ryvel)
        self.samples['fgxvel'].append(fd.fs.fgxvel)
        self.samples['fgyvel'].append(fd.fs.fgyvel)
        self.samples['fhxvel'].append(fd.fs.fhxvel)
        self.samples['fhyvel'].append(fd.fs.fhyvel)
        self.samples['frxvel'].append(fd.fs.frxvel)
        self.samples['fryvel'].append(fd.fs.fryvel)
        
