# -*- coding: utf-8 -*-
'''
Class that accumulates samples and returns them as a {name:ndarray} dict.
'''

import numpy as np
import edf_data


class SampleAccumulator(object):

    def __init__(self):
        self.d = {}

    def update(self, a):
        if len(self.d.keys()) == 0:
            self.d = dict((k, [v]) for k, v in a.iteritems())
        else:
            # For all fields in a that are also in dict
            all_keys = set(a.keys() + self.d.keys())
            for key in all_keys:
                if key in a.keys():
                    value = a[key]
                if key not in self.d.keys():
                    # key is not yet in d. add it
                    self.d[key] = [np.nan]*len(self.d[self.d.keys()[0]])
                if key not in a.keys():
                    # key is not in new data. value should be nan
                    value = np.nan
                self.d[key].extend([value])

    def get_dict(self, params=None):
        d = {}
        for key, value in self.d.iteritems():
            values = np.array(value)
            if len(values.shape) == 2 and values.shape[1] == 2:
                d['left_' + key] = values[:, edf_data.LEFT_EYE]
                d['right_' + key] = values[:, edf_data.RIGHT_EYE]
            else:
                d[key] = values
        self.d = {}
        return d
