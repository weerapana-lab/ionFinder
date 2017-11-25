//
//  sequestParams.cpp
//  ms2_anotator
//
//  Created by Aaron Maurais on 11/22/17.
//  Copyright © 2017 Aaron Maurais. All rights reserved.
//

#include "../include/sequestParams.hpp"

void seqpar::SequestParamsFile::initSmodMap()
{
	smodMap["add_C_terminus"] = "C_term";
	smodMap["add_N_terminus"] = "N_term";
	smodMap["add_G_Glycine"] = "G";
	smodMap["add_A_Alanine"] = "A";
	smodMap["add_S_Serine"] = "S";
	smodMap["add_P_Proline"] = "P";
	smodMap["add_V_Valine"] = "V";
	smodMap["add_T_Threonine"] = "T";
	smodMap["add_C_Cysteine"] = "C";
	smodMap["add_L_Leucine"] = "L";
	smodMap["add_I_Isoleucine"] = "I";
	smodMap["add_N_Asparagine"] = "N";
	smodMap["add_O_Ornithine"] = "O";
	smodMap["add_D_Aspartic_Acid"] = "D";
	smodMap["add_Q_Glutamine"] = "Q";
	smodMap["add_K_Lysine"] = "K";
	smodMap["add_E_Glutamic_Acid"] = "E";
	smodMap["add_M_Methionine"] = "M";
	smodMap["add_H_Histidine"] = "H";
	smodMap["add_F_Phenyalanine"] = "F";
	smodMap["add_R_Arginine"] = "R";
	smodMap["add_Y_Tyrosine"] = "Y";
	smodMap["add_W_Tryptophan"] = "W";
	
	smodMap["add_X_LorI"] = seqpar::BAD_SMOD;
	smodMap["add_B_avg_NandD"] = seqpar::BAD_SMOD;
	smodMap["add_Z_avg_QandE"] = seqpar::BAD_SMOD;
}

bool seqpar::SequestParamsFile::read(string _fname)
{
	if(fname.empty())
		throw runtime_error("fname must be specified!");
	
	utils::File file(fname);
	if(!file.read())
		return false;
	
	string line;
	vector<string> elems;
	while(!file.end())
	{
		line = utils::trim(file.getLine_skip_trim());
		if(utils::strContains('=', line))
		{
		
			utils::split(line, '=', elems);
			utils::trimAll(elems);
			
			if(smodMap.find(elems[0]) != smodMap.end())
			{
				string modMass = elems[1];
				string aa = smodMap[elems[0]];
				modMass = utils::trim(modMass.substr(0, modMass.find(";")));
				aaMap[aa] = aaDB::AminoAcid(aa, 0, utils::toDouble(modMass));
			}
			else if(elems[0] == DIFF_MOD_LINE)
			{
				string diffModsLine = elems[1];
				elems.clear();
				istringstream iss(diffModsLine);
				for(string s; iss >> s; )
					elems.push_back(s);
				
				if(elems.size() > 2)
				{
					cerr << "Multiple diffmods detected. Use smod file with multiple diffmods" << endl;
					return false;
				}
				aaMap["*"] = aaDB::AminoAcid("*", 0, utils::toDouble(elems[0]));
			}
		}
	}
	
	return true;
}







