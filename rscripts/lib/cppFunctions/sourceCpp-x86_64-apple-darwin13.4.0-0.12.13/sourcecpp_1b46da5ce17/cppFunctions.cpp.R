`.sourceCpp_1_DLLInfo` <- dyn.load('/Users/Aaron/scripts/ms2_anotator/rscripts/lib/cppFunctions/sourceCpp-x86_64-apple-darwin13.4.0-0.12.13/sourcecpp_1b46da5ce17/sourceCpp_4.so')

getSubscriptNum <- Rcpp:::sourceCppFunction(function(str) {}, FALSE, `.sourceCpp_1_DLLInfo`, 'sourceCpp_1_getSubscriptNum')
makeIonLabels <- Rcpp:::sourceCppFunction(function(letter, count, dir) {}, FALSE, `.sourceCpp_1_DLLInfo`, 'sourceCpp_1_makeIonLabels')
makeIonLabelsExp <- Rcpp:::sourceCppFunction(function(letter, count, dir) {}, FALSE, `.sourceCpp_1_DLLInfo`, 'sourceCpp_1_makeIonLabelsExp')
makeSeqVec <- Rcpp:::sourceCppFunction(function(seq) {}, FALSE, `.sourceCpp_1_DLLInfo`, 'sourceCpp_1_makeSeqVec')
isArg <- Rcpp:::sourceCppFunction(function(arg) {}, FALSE, `.sourceCpp_1_DLLInfo`, 'sourceCpp_1_isArg')
fixOD <- Rcpp:::sourceCppFunction(function(str) {}, FALSE, `.sourceCpp_1_DLLInfo`, 'sourceCpp_1_fixOD')

rm(`.sourceCpp_1_DLLInfo`)
