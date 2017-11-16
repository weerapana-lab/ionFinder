//
//  main.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 3/20/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include <iostream>
#include "utils.cpp"
#include "peptide.cpp"
#include "aaDB.cpp"
#include "ms2.cpp"
#include "geometry.cpp"
#include "calcLableLocs.cpp"

using namespace std;

int main()
{
	string seq_mod = "WC*AVSEHEATK";
	string seq = "WCAVSEHEATK";
	string modMassDBLoc = "/Users/Aaron/Xcode_projects/ms2_anotator/staticModifications.txt";
	string modTestDBLOC = "/Users/Aaron/Xcode_projects/ms2_anotator/staticModifications_diffModTest.txt";
	
	peptide::Peptide test(seq);
	peptide::Peptide modTest(seq_mod);
	
	modTest.initalize(modTestDBLOC);
	test.initalize(modMassDBLoc);
	
	//test.calcFragments(1, 1);
	//modTest.calcFragments(1, 1);
	
	ofstream outF1("/Users/Aaron/Xcode_projects/ms2_anotator/testFiles/testPeptide.txt");
	test.printFragments(outF1);
	ofstream outF2("/Users/Aaron/Xcode_projects/ms2_anotator/testFiles/testPeptide_mod.txt");
	modTest.printFragments(outF2);
	
	cout << "mass 1 is: " << test.getMass() << endl;
	cout << "mass 2 is: " << modTest.getMass() << endl;
	
	//string testFile = "20160418_Healthy_P1_SF_03.06689.06689.2";
	//string testFile = "/Users/Aaron/Xcode_projects/ms2_anotator/testFiles/fixedShort.ms2";
	string testFile = "/Users/Aaron/Xcode_projects/ms2_anotator/testFiles/20160418_Healthy_P1_SF_03.ms2";
	
	ms2::Ms2File file(testFile);
	if(!file.read())
		cout << "Failed to read!" << endl;
	else cout << "read in" << endl;
	
	ms2::Spectrum temp;
	if(!file.getScan(6353, temp))
		cout << "scan not found" << endl;
	else {
		temp.normalizeIonInts(100);
		temp.labelSpectrum(test, false);
		temp.calcLabelPos();
		ofstream outF("/Users/Aaron/Xcode_projects/ms2_anotator/testFiles/testScan.tsv");
		temp.printLabeledSpectrum(outF, false);
	}
	
	return 0;
}
