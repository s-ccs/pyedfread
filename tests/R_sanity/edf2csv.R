
library(eyelinkReader)
args <- commandArgs(trailingOnly = TRUE)
fn = args[1];
tag = args[2];
gaze <- read_edf(fn, import_samples=TRUE);
outfn = paste('df', string(tag), '.csv', sep='');
write.csv(gaze$samples, outfn);
