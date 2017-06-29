'''
Test pyedfread by comparing against edf2asc output.

Only compares readout of samples at the moment.
'''

import unittest
import numpy as np
from pyedfread import edf


class TestEdfread(unittest.TestCase):

    def setUp(self):
        samples, events, messages = edf.pread('../SUB001.EDF')
        self.samples = samples
        self.events = events
        self.messages = messages

    @unittest.skip('Takes too long')
    def test_validate_against_edf2asc(self):
        samples = self.samples.loc[:, ('time', 'gx_right', 'gy_right', 'pa_right')]
        samples = samples.round(1).set_index('time')
        with open('../SUB001.asc') as asc:
            for line in asc:
                x = line.strip().replace('...', '').replace('\t', '').replace('  ', ' ').split(' ')
                if len(x) == 4:
                    try:
                        # SAMPLE, X, Y, PA
                        x = [float(n) for n in x]
                        row = samples.loc[x[0], :]
                        self.assertTrue(np.isclose(x[1], row.gx_right)
                            and np.isclose(x[2], row.gy_right)
                            and np.isclose(x[3], row.pa_right))
                    except ValueError:
                        pass


    def test_ignore_samples(self):
        samples, events, messages = edf.pread('../SUB001.EDF', ignore_samples=True)
        self.assertEqual(samples.shape[0], 0)


    def test_messages(self):
        '''
        Compare messages to known values.
        '''
        self.assertTrue(all(
            (self.messages['sample'].values) == np.arange(6, 292, 5)))
        self.assertEqual(self.messages.shape, (58, 5))
        self.assertEqual(self.messages.trialid_time.loc[57], 502911)


    def test_events(self):
        '''
        Compare events to known values.
        '''
        self.assertEqual(self.events.shape, (485, 30))
        self.assertEqual(np.round(self.events.gavx.mean(), 3),
                                  np.round(420.842680343156, 3))
        self.assertEqual(np.round(self.events.gavy.mean(), 3),
                                  np.round(205.31814411595926, 3))





if __name__ == '__main__':
    unittest.main()
