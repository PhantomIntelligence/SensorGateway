// DETECTION ALGORITHM MUSIC (Algo5)
// Version 1 (2017)
// Original version : Simon E-L
// Modified by MA Isabel

#include <stdlib.h>
#include <math.h>
#include "awl_music.h"
#include "awl_fdist.h"

#define DEFAULT_SIGNAL_LENGTH    MAX_TRACK// Default: 350
#define NBSUBMODELS    17
#define LENGTHSUBMODELS    27
#define PSSTEP    1
#define SUBMODELDEPTH    1 / ( NBSUBMODELS - 1)
#define PRECISION    0.05
#define RMSWINDOWSIZE    30
//#define NBCHANNELS    awl->num_channels

void validateMUSICParameters();
int insertCurrentChannelSignalInHistoryBuffer(int32_t* Y, int _currChan);
int reportDetection(int _currChan);
void GetRxx();
void ComputeEigenVectorFromRxx();
unsigned int GetNumberOfSourcesFromEigenValues(float *eigenValuesArray,
		unsigned int eVSize);
void findmaxMUSIC(float *fResults, float *fVector, unsigned int uiStart,
		unsigned int uiEnd);
void GetPeaks(float *fD, float *fPS, unsigned int uiMaxDetect, int iStart,
		int iEnd, int iPSstep);
//void GetTargetsSNR(int, float *, float *, int, float *);
void GetTargetsSNR2(float *, float *, int, float *);
void GetPn(awl_data *awl);
void GetPS(awl_data *awl);
void GetSubBin(awl_data *awl);


// MUSIC SUB MODELS TABLE (Contient tous les submodels de taille LENGTHSUBMODELS)
float fSUBMODELS[459] = 
	  { 0.0006, 0.0043, 0.013, 0.0318, 0.0656, 0.1224, 0.2080,
		0.3265, 0.4748, 0.64, 0.8002, 0.9285, 1, 1, 0.9285, 0.8002, 0.64,
		0.4748, 0.3265, 0.2080, 0.1226, 0.0669, 0.0337, 0.0155, 0.0065, 0.0023,
		0.0007, 0.0006, 0.0041, 0.0124, 0.0303, 0.0628, 0.1178, 0.2012, 0.3174,
		0.4638, 0.6282, 0.7891, 0.9199, 0.9954, 1, 0.9328, 0.8077, 0.649,
		0.4837, 0.3342, 0.2139, 0.1267, 0.0694, 0.0351, 0.0163, 0.0068, 0.0025,
		0.0007, 0.0005, 0.0039, 0.0118, 0.0289, 0.0602, 0.1135, 0.1947, 0.3086,
		0.4531, 0.6166, 0.7782, 0.9114, 0.9908, 1, 0.9371, 0.8152, 0.6582,
		0.4929, 0.3422, 0.22, 0.131, 0.0721, 0.0367, 0.0171, 0.0072, 0.0026,
		0.0008, 0.0005, 0.0037, 0.0112, 0.0276, 0.0577, 0.1093, 0.1884, 0.3001,
		0.4426, 0.6052, 0.7674, 0.9030, 0.9862, 1, 0.9415, 0.8229, 0.6674,
		0.5022, 0.3503, 0.2264, 0.1354, 0.0749, 0.0383, 0.0179, 0.0076, 0.0028,
		0.0008, 0.0005, 0.0035, 0.0106, 0.0263, 0.0553, 0.1052, 0.1823, 0.2917,
		0.4324, 0.594, 0.7568, 0.8946, 0.9816, 1, 0.9459, 0.8305, 0.6768,
		0.5116, 0.3586, 0.2328, 0.14, 0.0778, 0.04, 0.0188, 0.008, 0.0029,
		0.0009, 0.0005, 0.0033, 0.0101, 0.0251, 0.053, 0.1013, 0.1764, 0.2836,
		0.4224, 0.5831, 0.7463, 0.8863, 0.9771, 1, 0.9503, 0.8383, 0.6863,
		0.5213, 0.3671, 0.2395, 0.1447, 0.0808, 0.0417, 0.0197, 0.0084, 0.0031,
		0.0009, 0.0004, 0.0031, 0.0096, 0.0240, 0.0508, 0.0975, 0.1706, 0.2758,
		0.4127, 0.5723, 0.736, 0.8782, 0.9726, 1, 0.9547, 0.8461, 0.696, 0.5311,
		0.3758, 0.2464, 0.1495, 0.084, 0.0435, 0.0207, 0.0089, 0.0033, 0.001,
		0.0004, 0.003, 0.0091, 0.0229, 0.0487, 0.0939, 0.1651, 0.2681, 0.4031,
		0.5617, 0.7258, 0.87, 0.9681, 1, 0.9591, 0.854, 0.7058, 0.5411, 0.3847,
		0.2534, 0.1546, 0.0872, 0.0454, 0.0217, 0.0094, 0.0035, 0.0011, 0.0004,
		0.0028, 0.0087, 0.0218, 0.0467, 0.0904, 0.1597, 0.2606, 0.3938, 0.5513,
		0.7157, 0.862, 0.9636, 1, 0.9636, 0.862, 0.7157, 0.5513, 0.3938, 0.2606,
		0.1597, 0.00906, 0.0474, 0.0227, 0.0099, 0.0037, 0.0011, 0.0004, 0.0027,
		0.0083, 0.0208, 0.0448, 0.0871, 0.1546, 0.2534, 0.3847, 0.5411, 0.7058,
		0.854, 0.9591, 1, 0.9681, 0.87, 0.7258, 0.5617, 0.4031, 0.2681, 0.1651,
		0.0941, 0.0495, 0.0238, 0.0104, 0.0039, 0.0012, 0.0003, 0.0025, 0.0078,
		0.0199, 0.0429, 0.0838, 0.1495, 0.2464, 0.3758, 0.5311, 0.696, 0.8461,
		0.9547, 1, 0.9726, 0.8782, 0.736, 0.5723, 0.4127, 0.2758, 0.1706,
		0.0977, 0.0517, 0.025, 0.011, 0.0041, 0.0013, 0.0003, 0.0024, 0.0075,
		0.019, 0.0411, 0.0807, 0.1447, 0.2395, 0.3671, 0.5213, 0.6863, 0.8383,
		0.9503, 1, 0.9771, 0.8863, 0.7463, 0.5831, 0.4224, 0.2836, 0.1764,
		0.1015, 0.0539, 0.0262, 0.0115, 0.0044, 0.0013, 0.0003, 0.0023, 0.0071,
		0.0181, 0.0394, 0.0777, 0.14, 0.2328, 0.3586, 0.5116, 0.6768, 0.8305,
		0.9459, 1, 0.9816, 0.8946, 0.7568, 0.594, 0.4324, 0.2917, 0.1823,
		0.1054, 0.0563, 0.0275, 0.0122, 0.0046, 0.0014, 0.0003, 0.0021, 0.0067,
		0.0173, 0.0377, 0.0748, 0.1354, 0.2264, 0.3503, 0.5022, 0.6674, 0.8229,
		0.9415, 1, 0.9862, 0.903, 0.7674, 0.6052, 0.4426, 0.3001, 0.1884,
		0.1095, 0.0587, 0.0288, 0.0128, 0.0049, 0.0015, 0.0003, 0.002, 0.0064,
		0.0165, 0.0362, 0.072, 0.131, 0.22, 0.3422, 0.4929, 0.6582, 0.8152,
		0.9371, 1, 0.9908, 0.9114, 0.7782, 0.6166, 0.4531, 0.3086, 0.1947,
		0.1137, 0.0613, 0.0302, 0.0135, 0.0052, 0.0016, 0.0003, 0.0019, 0.0061,
		0.0157, 0.0347, 0.0694, 0.1267, 0.2139, 0.3342, 0.4837, 0.649, 0.8077,
		0.9328, 1, 0.9954, 0.9199, 0.7891, 0.6282, 0.4638, 0.3174, 0.2012,
		0.1181, 0.064, 0.0317, 0.0142, 0.0055, 0.0017, 0, 0.0006, 0.0043, 0.013,
		0.0318, 0.0656, 0.1224, 0.208, 0.3265, 0.4748, 0.64, 0.8002, 0.9285, 1,
		1, 0.9285, 0.8002, 0.64, 0.4748, 0.3265, 0.208, 0.1226, 0.0669, 0.0337,
		0.0155, 0.0065, 0.0023 };

float fSignalsBuffer[DEFAULT_SIGNAL_LENGTH * (int)ALGO5_PARAM4_HISTORYLENGTH * AWL_MAX_CHANNELS] = { 0 };
float *fSIGNALS;
float fRXX[(int)ALGO5_PARAM2_NBDIAGS * DEFAULT_SIGNAL_LENGTH] = { 0 };
float fAMPDIAGS[17] = { 1, 0.9993, 0.9452, 0.8614, 0.7564, 0.6398, 0.5215,
		0.4092, 0.3094, 0.2252, 0.1579, 0.1065, 0.0692, 0.0432, 0.026, 0.015,
		0.0083 }; // Pour modulation par matrice modèle, pas pour le pseudospectre
float fV0[DEFAULT_SIGNAL_LENGTH] = { 0 };
float fVECTP[DEFAULT_SIGNAL_LENGTH * DEFAULT_AWL_MAX_DETECTED] = { 0 };
float fVALP[DEFAULT_AWL_MAX_DETECTED] = { 0 };
float *fPN = fRXX;
float *fPSINT1 = fVECTP;
float *fPS = fV0;
float *fD = fVALP;
float detectionSNRvalues[DEFAULT_AWL_MAX_DETECTED] = { 0 };
float *fMODELTEMP = fVECTP + DEFAULT_SIGNAL_LENGTH;
float *fPSINT2 = fVECTP + 2 * DEFAULT_SIGNAL_LENGTH;
static awl_data* awl;
static int signalLength;


static float offset = 0.0;
static int sourcewidth = 7;  // half width
static int nbdiags = 8;
static int maxCountEVD = 10;
static int history_length = 4;
static int maxNumberOfSources = 0; // 0 = Self determined
static float snrMin = 0.0;

int start;
// fixed
int iMODELHWF = 0.5 * (LENGTHSUBMODELS - 1);

/* Fonction principale appelée de l'extérieur pour faire rouler MUSIC*/
int music(int32_t Y[], int nb_point, awl_data *_awl) {

	int channel;
	int nb_detected = 0;

	awl = _awl;
	channel = GET_CURRENT_AWL_CHANNEL;

	signalLength = MAX_TRACK - awl->dead_zone;

	offset = ALGO_PARAM(0);
	sourcewidth = (int)ALGO_PARAM(1);
	nbdiags = (int)ALGO_PARAM(2);
	maxCountEVD = (int)ALGO_PARAM(3);
	history_length = (int)ALGO_PARAM(4);
	maxNumberOfSources =(int)ALGO_PARAM(5);
	snrMin = ALGO_PARAM(7);
	

	validateMUSICParameters();

	if (insertCurrentChannelSignalInHistoryBuffer(Y, channel))
	{
		fSIGNALS = &fSignalsBuffer[((int) channel) * signalLength
				* history_length];
		//fSIGNALS = yscale;
		GetRxx();
		ComputeEigenVectorFromRxx();
		GetPn(awl);
		GetPS(awl);
		//GetTargetsSNR((int)signalLength, fSIGNALS, &fD[0], (int)awl->maxNumberOfDetections, &detectionSNRvalues[0]);
		GetTargetsSNR2(fSIGNALS, &fD[0], (int) awl->maxNumberOfDetections,
				&detectionSNRvalues[0]);

		GetSubBin(awl);
		nb_detected = reportDetection(channel);
	}
	return nb_detected;
}

/* Parameters validation function */
void validateMUSICParameters() 
{
	if (nbdiags > ALGO5_PARAM2_NBDIAGS) { // Saturations
		nbdiags = ALGO5_PARAM2_NBDIAGS;
	} else if (nbdiags <= 0) {
		nbdiags = 1;
	}

	if (history_length > ALGO5_PARAM4_HISTORYLENGTH) {
		history_length = ALGO5_PARAM4_HISTORYLENGTH;
	} else if (history_length <= 0) {
		history_length = 1;
	}

	start = 0;
	if (awl->dead_zone <= 146) {
		start = 146 - awl->dead_zone;
	}

	printf(
			"Algo5 parameters: %f %d %d %d %f\n",
			offset, sourcewidth, nbdiags, history_length, snrMin);

}

int reportDetection(int _currChan)
 {
	int i;
	int detected = 0;
	for (i = 0; i < awl->maxNumberOfDetections; i++) {
		if ((fD[i] > 0) && (detectionSNRvalues[i] > snrMin)) 
		{
			fD[i] = (SAMPLING_PERIOD / LIGHTSPEED * fD[i])
					- offset;
			awl->detected[_currChan][detected].meters = fD[i];
			awl->detected[_currChan][detected].intensity = 2*(detectionSNRvalues[i] + 21);
			detected++;
		}
	}
	return (detected);
}

void findmaxMUSIC(float *fResults, float *fVector, unsigned int uiStart,
		unsigned int uiEnd) {
			
	fResults[0] = 0;
	fResults[1] = 0;

	findmax_and_pos(fResults, fVector + uiStart, uiEnd - uiStart);
}

// Fill the X matrix with each channel
int insertCurrentChannelSignalInHistoryBuffer(int32_t* Y, int _currChan) 
{
	int i;
	static int iSignalCountEachChannel[AWL_MAX_CHANNELS] = { 0 };
	int readyToProcess = 0;

	for (i = 0; i < signalLength; i++) {
		//yscale[i] =  ( float ) ( Y[i+start] );
		fSignalsBuffer[i + (iSignalCountEachChannel[(int) _currChan] * signalLength)
				+ (((int) _currChan) * signalLength * history_length)] =
				(float) (Y[i + start]);
	}
	if (++iSignalCountEachChannel[(int) _currChan] == history_length) {
		iSignalCountEachChannel[(int) _currChan] = 0;
		readyToProcess = 1;
	}
	return (readyToProcess);
}

/* Génération d'une approximation de Rxx */
void GetRxx() 
{
	int i, j, iDiagEven, iDiagEven2, iDiagOdd, iDiagOdd2;
	int iTotalHistoryLength = signalLength * history_length;
	register float *fPSignal, *fPDiagRxx, *fPDiagRxx2, *fPDiagRxxEven, *fPDiagRxxOdd, fSignalValue, fDiagRxxValue, fValueTemp=0;

	/* Remplir Rxx(1,1) et Rxx(end,end) */
	// On récupère la première valeur de chaque signaux dans l'historique X, on la met au carré et on l'additionne dans fValueTemp2, soit la valeur de Rxx(1,1)                                // Valeur de RXX(0,0)
	fPSignal = fSIGNALS;
	for (j = 0; j < iTotalHistoryLength; j += signalLength) {
		fSignalValue = *fPSignal; // Evite d'aller lire 2 fois la valeur contenu de dPtemp1
		 fValueTemp += fSignalValue * fSignalValue;
		fPSignal += signalLength;
	}

	fRXX[0] = fValueTemp;

	/* Remplir fRxx0 (diag principale) et fRxx1 (1ere diag secondaire) */
	fPDiagRxx = fRXX; // Adresse première valeur de la diagonale principale
	fPDiagRxx2 = fRXX + signalLength; // Adresse première valeur de la première diagonale secondaire
	for (i = 1; i < signalLength; i++)
	{
		fDiagRxxValue = 0;
		fPSignal = fSIGNALS + i; // Pointer sur le tableau de signaux
		for (j = 0; j < iTotalHistoryLength; j += signalLength) // Diagonale principale
		{
			fValueTemp = *fPSignal;
			fDiagRxxValue += fValueTemp * fValueTemp;
			fPSignal += signalLength;
		}
		*fPDiagRxx2++ = fAMPDIAGS[1] * 0.5 * (*fPDiagRxx + fDiagRxxValue); // Approximation pour l'obtention de la seconde diagonale
		fPDiagRxx += 1;
		*fPDiagRxx = fDiagRxxValue;
	}

	/* Remplir et moduler les autres diagonales */
	iDiagEven = 2;                                     
	iDiagOdd = 3;                                    
	while (iDiagEven < nbdiags && iDiagOdd < nbdiags )
	{

		fPDiagRxx = fRXX + iDiagEven * signalLength; // Pointe la diagonale Rxx_{d_iPaire}
		fPDiagRxx2 = fRXX + iDiagOdd * signalLength;
		iDiagEven2 = 0.5*iDiagEven;
		iDiagOdd2 = 0.5*(iDiagOdd - 1);
		fPDiagRxxEven = fRXX + iDiagEven2;
		fPDiagRxxOdd = fRXX + signalLength + iDiagOdd2;

		for (j = 0; j < signalLength - iDiagEven; j++) // Approximation des diagonales à partir de la principale
		{
			*fPDiagRxx++ = fAMPDIAGS[iDiagEven] * *fPDiagRxxEven++;
			*fPDiagRxx2++ = fAMPDIAGS[iDiagOdd] * *fPDiagRxxOdd++;
		}
		iDiagEven += 2;
		iDiagOdd += 2;
	}
}

/* Décomposition propre, fV0 = vecteur sur lequel on boucle, fVectP = tableaux 
	de vecteurs propres */
void ComputeEigenVectorFromRxx() 
{

	int i, j, k, l;
	float fMAXResults[2] = { 0 };
	float *fPVectInit, *fPDiagRxx, *fPEigenVect, *fPEigenVect2, *fPOldV0Value, fNewV0Value, fEigenValue, invValP, fSumSquared;

	for (i = 0; i < awl->maxNumberOfDetections; i++) { // Boucle sur les valeurs/vecteurs propres

		fPVectInit = fV0; // Assignation de l'adresse du vecteur propre initial
		for (j = 0; j < signalLength; j++) // Initialisation du vecteur de depart
		{ 
			if (((j + i) % 30) == 0) {
				*fPVectInit++ = 1;
			} else {
				*fPVectInit++ = 0;
			}
		
		}

		// for (j = 0; j < signalLength; j++) { // Initialisation du vecteur de depart
		// 	*fPtemp1++ = 1;
		// }

		if (i > 0) { // Élimine les valeurs négatives de R, la matrice de covariance, ne le fait pas surla matrice initiale, seulement lorsque des contributions sont soustraites
			for (k = 0; k < nbdiags ; k++) {

				fPDiagRxx = fRXX + k * signalLength; // Assignation de l'adresse de fRXX (diagonales de la matrice) dans fPtemp1

				for (j = 0; j < signalLength; j++) // Filtre pour éliminer les valeurs négatives
				{
					if (*fPDiagRxx < 0) {
						*fPDiagRxx = 0;
					}
					fPDiagRxx++;
				}
			}
		}



		for (j = 0; j < maxCountEVD; j++) // Boucle maxCountEVD fois pour obtenir le vecteur propre i
		{
			// fVECTP -> Tableau qui contient les awl->maxNumberOfDetections vecteurs propres

			fPEigenVect = fVECTP + (i * signalLength) + nbdiags - 1;

			for (k = nbdiags - 1; k < signalLength - nbdiags ; k++) // Boucle (v = Rxx*v0) 
			{
				/* Chaque itération k permet d'ajouter une portion supplémentaires des diagonales et donc de remplir
				   une case supplémentaire du vecteur initial fV0
				*/
				fNewV0Value = 0;
				fPOldV0Value = fV0 + k - (nbdiags - 1); // Pointer qui commence au debut de fV0
				fPDiagRxx = fRXX + k - (nbdiags - 1) + (nbdiags-1)*signalLength; // Pointer qui commence au début de la dernière diagonale pour fRxx
				
				for (l = -(nbdiags - 1); l <= 0; l++) // Multiplication de la portion *inférieure* de Rxx
				{
					fNewV0Value += *fPDiagRxx++ * *fPOldV0Value++;
					fPDiagRxx -= signalLength; // On va chercher la prochaine première valeur du vecteur
				}

				fPDiagRxx = fRXX + k + signalLength; // Pointe sur la première valeur de la seconde diagonale

				for (l = 1; l < nbdiags ; l++) 
				{
					fNewV0Value += *fPDiagRxx * *fPOldV0Value++;
					fPDiagRxx += signalLength; // On va chercher la prochaine diagonale
				}
				
				*fPEigenVect++ = fNewV0Value; // Valeur du vecteur propre est placée en mémoire

			}

			fMAXResults[0] = 0;
			fMAXResults[1] = 0;

			findmaxMUSIC(fMAXResults, fVECTP, i * signalLength, // Normalisation du signal par valeur max, fresults contient la valeur et la position dans fVECTP du max
					(i + 1) * signalLength);

			fVALP[i] = fMAXResults[0]; // Stockage de la valeur propre

			invValP = 1.0 / fVALP[i]; // Inverse de la valeur propre pour normalisation (division = opération plus coûteuse)

			/* Normalisation par le maximum du vecteur propre */
			if (j < (maxCountEVD - 1)) 
			{
				fPVectInit = fV0;
				fPEigenVect = fVECTP + i * signalLength;
				for (k = 0; k < signalLength; k++) {
					*fPVectInit++ = invValP * *fPEigenVect;
					*fPEigenVect++ = 0;
				}
			}

		}

		/* Normalisation par la norme euclidienne */

		fSumSquared = 0;
		fPEigenVect = fVECTP + i * signalLength;
		for (j = 0; j < signalLength; j++) {
			fSumSquared += *fPEigenVect * *fPEigenVect;
			fPEigenVect++;
		}

		fSumSquared = sqrtf(fSumSquared);
		fSumSquared = 1.0 / fSumSquared;

		fPEigenVect = fVECTP + i * signalLength;
		for (j = 0; j < signalLength; j++)
		{
			*fPEigenVect++ *= fSumSquared;
		}

		/* Soustraction de la contribution */
		fEigenValue = fVALP[i];
		for (j = 0; j < nbdiags ; j++) {
			fPDiagRxx = fRXX + j * signalLength;
			fPEigenVect = fVECTP + i * signalLength;
			fPEigenVect2 = fVECTP + i * signalLength + j;
			for (k = 0; k < signalLength - j; k++) {
				*fPDiagRxx++ -= fEigenValue * *fPEigenVect++ * *fPEigenVect2++;
			}
		}

	}

}

unsigned int GetNumberOfSourcesFromEigenValues(float *eigenValuesArray,
		unsigned int eVSize) {

	unsigned int indexToValidate;

	for (indexToValidate = 1; indexToValidate < eVSize; indexToValidate++) // Boucler sur l'array de valeurs propres
	 {
		if (eigenValuesArray[indexToValidate] <= 0.5 * eigenValuesArray[0]) // Critère de validation
		 {
			break;
		}
	}
	return indexToValidate;
}

void GetPn(awl_data *awl) // Obtention du projecteur de l'espace bruit
{

	int i, j, k, iM;
	int iNBDIAGSmSIGNALS = nbdiags * signalLength;
	register float *fPVectPn, *fPEigenVect, *fPEigenVect2;

	iM = maxNumberOfSources;
	if(!iM) {
		iM = GetNumberOfSourcesFromEigenValues(fVALP, awl->maxNumberOfDetections);
	}

	fPVectPn = fPN;
	for (i = 0; i < signalLength; i++) 
	{        // Initialisation comme matrice identitée (diagonale principale = 1)
		*fPVectPn++ = 1; 
	}

	for (i = signalLength; i < iNBDIAGSmSIGNALS; i++) 
	{		// Initialisation comme matrice identitée (diagonales secondaires = 0)
		*fPVectPn++ = 0;
	}

	/* Pn = I-Es*Es' */
	for (i = 0; i < nbdiags ; i++) // AJustement du décalage en fonction de la diagonale
	{
		for (j = 0; j < iM; j++) // Boucle sur les valeurs propres (pour savoir combien de vecteurs propres à inclure)
		{
			fPVectPn = fPN + i * signalLength;
			fPEigenVect = fVECTP + j * signalLength;
			fPEigenVect2 = fVECTP + j * signalLength + i;
			for (k = 0; k < signalLength - i; k++) // Fait toute la diagonale 
			{
				*fPVectPn++ -= 1.0321 * *fPEigenVect++ * *fPEigenVect2++; // Terme hardcoded bizarre
				//*fPtemp1++ -= 0.5 * *fPtemp2++ * *fPtemp3++;
			}
		}
	}

	//printf("Pn array\n");
	//for (i=0;i<signalLength;i++) printf("%f  ",fPN[i]);
	//printf("\n\n");

	return;
}

void GetPeaks(float *fD, float *fPS, unsigned int uiMaxDetect, int iStart,
		int iEnd, int iPSSTEP) {

	int i;
	unsigned int uiCompte = 0;

	i = iStart;
	while (i < iEnd) {

		if ((fPS[i] > fPS[i - iPSSTEP])
				&& (fPS[i] > fPS[i + iPSSTEP]) & (fPS[i] > 0.5)
						& (uiCompte < uiMaxDetect)) {
			fD[uiCompte] = i;
			uiCompte += 1;
		}

		i += iPSSTEP;
	}

	return;
}


 /* Obtention du pseudospectre de MUSIC */
void GetPS(awl_data *awl) 
{

	int i, j, k, iPosModels, iPosD, iDcompte;

	register float *fPVectPS, *fPVectPS2, *fPVectInt1, *fPVectInt2, *fPVectModel, fVectInt1Value,
			fVectInt2Value, fVectPSValue, fVectPS2Value;

	iPosModels = 0.5 * (NBSUBMODELS - 1);

	fPVectPS = fPS;
	for (i = 0; i < signalLength; i++) // reinitialise vector
	{
		*fPVectPS++ = 0;
	}

	/* Tracage du pseudospectre */
	/* L'obtention de chaque point du pseudospectre de [iMODELHWF à signalLength - iMODELHWF]
		passe par la multiplication du modèle par Pn pour former un 'vecteur intermédiaire' (PSINT). Ce
		vecteur est ensuite remultiplier par le modèle pour obtenir la valeur PS(i), soit la
		valeur du pseudospectre au point i */

	fPVectPS = fPS + iMODELHWF;
	i = iMODELHWF;

	while (i < signalLength - iMODELHWF) // Balayage de la position centrale du modèle dans PS
	{
		/** Calcul de PSint **/ // int = intermediaire
		// La valeur de k permet d'une part, de compenser pour le déplacement sur un diagonale et,
		// d'autre part, de modifier la diagonale en cours
		/* PORTION INFÉRIEURE DU MODÈLE j<0 */

		fPVectInt1 = fPSINT1; // Pointe sur vecteurs propres
		for (j = -iMODELHWF; j < 0; j++) 
		{
			fVectInt1Value = 0;
			fPVectModel = fSUBMODELS + iMODELHWF
					- (nbdiags - 1)+ iPosModels*LENGTHSUBMODELS; // Adresse initiale + Position dans le modèle + Sélection du modèle central


			for (k = -(nbdiags - 1); k < nbdiags ; k++)
			{
				if (j - k > 0) {
					fVectInt1Value += *fPVectModel
							* fPN[i + k + (j - k) * signalLength];
				} else if (k - j < nbdiags) {
					fVectInt1Value += *fPVectModel
							* fPN[i + j + (k - j) * signalLength];
				}
				fPVectModel++; // Incrément de la case du modèle
			}
			*fPVectInt1++ = fVectInt1Value;
		}

		/* POINT CENTRAL DU MODÈLE j==0 */
		
		fVectInt1Value = 0;
		fPVectModel = fSUBMODELS + iMODELHWF
				- (nbdiags - 1)+ iPosModels*LENGTHSUBMODELS;

		for (k = -(nbdiags - 1); k < 0; k++) {
			fVectInt1Value += *fPVectModel++ * fPN[i + k + -k * signalLength];
		}
		for (k = 0; k < nbdiags ; k++) {
			fVectInt1Value += *fPVectModel++ * fPN[i + k * signalLength];
		}
		*fPVectInt1++ = fVectInt1Value;

		/* PORTION SUPÉRIEURE DU MODÈLE j>0 */

		for (j = 1; j <= iMODELHWF; j++) 
		{
			fVectInt1Value = 0;
			fPVectModel = fSUBMODELS + iMODELHWF
					- (nbdiags - 1) + iPosModels*LENGTHSUBMODELS; 
			
			for (k = -(nbdiags - 1); k < nbdiags ; k++) 
			{
				if (k - j >= 0) {
					fVectInt1Value += *fPVectModel
							* fPN[i + j + (k - j) * signalLength];
				} else if (j - k < nbdiags) 
				{
					fVectInt1Value += *fPVectModel
							* fPN[i + k + (j - k) * signalLength];
				}
				fPVectModel++;
			}
			*fPVectInt1++ = fVectInt1Value;
		}


		/** Calcul de PS(i): Remultiplication par le modèle **/
		fPVectInt1 = fPSINT1;
		fPVectModel = fSUBMODELS + iPosModels * LENGTHSUBMODELS;
		fVectPSValue = 0;
		for (j = 0; j < LENGTHSUBMODELS; j++) 
		{
			fVectPSValue += *fPVectInt1++ * *fPVectModel++;
		}
		*fPVectPS = fVectPSValue;

		if (*fPVectPS < 0) 
		{
			*fPVectPS = (float) abs((int) *fPVectPS);
		}

		*fPVectPS = 1.0 / (*fPVectPS);
		fPVectPS += PSSTEP;
		i += PSSTEP;
	}

	/* Detection des sommets */
	for (i = 0; i < awl->maxNumberOfDetections; i++) {
		fD[i] = 0;
	}

	GetPeaks(fD, fPS, awl->maxNumberOfDetections, iMODELHWF, signalLength - iMODELHWF,
	PSSTEP);

	/* Tracer le pseudospectre de chaque côté des sommets */
	/* Permet de calculer les points -PSSTEP/2 et +PSSTEP/2 autour de chaque détection */
	
	for (iDcompte = 0; iDcompte < awl->maxNumberOfDetections; iDcompte++) 
	{
		if (fD[iDcompte] > 0) 
		{

			iPosD = (int) fD[iDcompte];

			/* C�t�s gauche et droit du pic */

			fPVectPS = fPS + iPosD - (PSSTEP / 2); // ??????????????????? Quelle est l'adresse dans ce cas (le -0.5 rend ça ambigue)
			fPVectPS2 = fPS + iPosD + (PSSTEP / 2);

			for (i = -(PSSTEP / 2); i < 0; i++) 
			{

				/** Calcul de PSint **/

				/* j<0 */

				fPVectInt1 = fPSINT1;
				fPVectInt2 = fPSINT2;
				for (j = -iMODELHWF; j < 0; j++) 
				{
					fVectInt1Value = 0;
					fVectInt2Value = 0;
					fPVectModel = fSUBMODELS + iMODELHWF
							- (nbdiags - 1)+ iPosModels*LENGTHSUBMODELS;

					for (k = -(nbdiags - 1); k < nbdiags ; k++) {
						if (j - k > 0) {
							fVectInt1Value += *fPVectModel
									* fPN[iPosD + i + k
											+ (j - k) * signalLength];
							fVectInt2Value += *fPVectModel
									* fPN[iPosD - i + k
											+ (j - k) * signalLength];
						} else if (k - j < nbdiags) {
							fVectInt1Value += *fPVectModel
									* fPN[iPosD + i + j
											+ (k - j) * signalLength];
							fVectInt2Value += *fPVectModel
									* fPN[iPosD - i + j
											+ (k - j) * signalLength];
						}
						fPVectModel++;
					}
					*fPVectInt1++ = fVectInt1Value;
					*fPVectInt2++ = fVectInt2Value;
				}

				/* j==0 */
				fVectInt1Value = 0;
				fVectInt2Value = 0;
				fPVectModel = fSUBMODELS + iMODELHWF
						- (nbdiags - 1)+ iPosModels*LENGTHSUBMODELS;
				for (k = -(nbdiags - 1); k < 0; k++) {
					fVectInt1Value += *fPVectModel
							* fPN[iPosD + i + k + -k * signalLength];
					fVectInt2Value += *fPVectModel
							* fPN[iPosD - i + k + -k * signalLength];
					fPVectModel++;
				}
				for (k = 0; k < nbdiags ; k++) {
					fVectInt1Value += *fPVectModel
							* fPN[iPosD + i + k * signalLength];
					fVectInt2Value += *fPVectModel
							* fPN[iPosD - i + k * signalLength];
					fPVectModel++;
				}
				*fPVectInt1++ = fVectInt1Value;
				*fPVectInt2++ = fVectInt2Value;

				/* j>0 */
				for (j = 1; j <= iMODELHWF; j++) {
					fVectInt1Value = 0;
					fVectInt2Value = 0;
					fPVectModel = fSUBMODELS + iMODELHWF
							- (nbdiags - 1)+ iPosModels*LENGTHSUBMODELS;
					for (k = -(nbdiags - 1); k < nbdiags ; k++) {
						if (k - j >= 0) {
							fVectInt1Value += *fPVectModel
									* fPN[iPosD + i + j
											+ (k - j) * signalLength];
							fVectInt2Value += *fPVectModel
									* fPN[iPosD - i + j
											+ (k - j) * signalLength];
						} else if (j - k < nbdiags) {
							fVectInt1Value += *fPVectModel
									* fPN[iPosD + i + k
											+ (j - k) * signalLength];
							fVectInt2Value += *fPVectModel
									* fPN[iPosD - i + k
											+ (j - k) * signalLength];
						}
						fPVectModel++;
					}
					*fPVectInt1++ = fVectInt1Value;
					*fPVectInt2++ = fVectInt2Value;
				}

				/** Calcul de PS(i) **/
				fPVectInt1 = fPSINT1;
				fPVectInt2 = fPSINT2;
				fPVectModel = fSUBMODELS + iPosModels * LENGTHSUBMODELS;
				fVectPSValue = 0;
				for (j = 0; j < LENGTHSUBMODELS; j++) {
					fVectPSValue += *fPVectInt1++ * *fPVectModel;
					fVectPS2Value += *fPVectInt2++ * *fPVectModel;
					fPVectModel++;
				}
				*fPVectPS = fVectPSValue;
				*fPVectPS2 = fVectPS2Value;

				if (*fPVectPS < 0) {
					*fPVectPS = (float) abs((int) *fPVectPS);
				}
				if (*fPVectPS2 < 0) {
					*fPVectPS2 = (float) abs((int) *fPVectPS2);
				}

				*fPVectPS = 1.0 / (*fPVectPS);
				fPVectPS++;
				*fPVectPS2 = 1.0 / (*fPVectPS2);
				fPVectPS2--;
			}

		}
	}

	/* D�tection des sommets exacts */

	for (i = 0; i < awl->maxNumberOfDetections; i++) {
		if (fD[i] > 0) {
			GetPeaks(fD + i, fPS, 1, (int) fD[i] - (PSSTEP / 2),
					(int) fD[i] + (PSSTEP / 2), 1);
		}
	}
 
}

void GetTargetsSNR2(float *fSignal, float *fDistancesRound, int iMaxTargets,
		float *_SNRvalues) {

	float fSignalPower[DEFAULT_SIGNAL_LENGTH] = { 0 }, fMean = 0, fRMStemp, fSignalRMS =
			999999999999, fPeakPower, fPowerTemp;
	int i, j, iRMSwindowsize;

	for (i = signalLength - 50; i < signalLength; i++) {
		fMean += fSignal[i];
	}
	fMean /= (float) (i - (signalLength - 50));
	//if(iScanCount>1999){ printf("fMean : %f \n",fMean); }

	/* Calcul du RMS signal via une fen�tre glissante de largeur RMSWINDOWSIZE */
	iRMSwindowsize = (int) RMSWINDOWSIZE;

	for (i = signalLength - (iRMSwindowsize + 1) / 2 - 50;
			i <= signalLength - (iRMSwindowsize + 1) / 2; i++) {
		fRMStemp = 0;
		for (j = -(iRMSwindowsize / 2); j <= iRMSwindowsize / 2; j++) {
			fRMStemp += (fSignal[i + j] - fMean) * (fSignal[i + j] - fMean);
		}
		fRMStemp = sqrtf(fRMStemp / ((float) iRMSwindowsize));
		if (fRMStemp < fSignalRMS) {
			fSignalRMS = fRMStemp;
		}
	}

	for (i = sourcewidth; i < signalLength - sourcewidth; i++) {
		for (j = -sourcewidth; j <= sourcewidth; j++) {
			fSignalPower[i] += (fSignal[i + j] - fMean)
					* (fSignal[i + j] - fMean);
		}
	}

	for (i = 0; i < iMaxTargets; i++) {
		if (fDistancesRound[i] > 0) {
			fPeakPower = 0;
			for (j = -3; j <= 3; j++) {
				fPeakPower += fSignalPower[(int) fDistancesRound[i] + j];
			}
			fPeakPower /= ((double) sourcewidth);
			fPowerTemp = sqrtf(fPeakPower / (1 + 2 * sourcewidth));
			_SNRvalues[i] = 20 * log10f(1.25 * fPowerTemp / fSignalRMS);
		}
	}

	return;
}

void GetSubBin(awl_data *awl) {

	unsigned int uiPos, uiModelsStep, uiModelPosition, uiIDephMax;
	int i, j, k, iPosOverNeg;
	float fPSsub[3], fDephInt[3], fSubStep, fPSMAX, *fPVectInt1, *fPVectInt2, *fPVectSubModel, *fPVectSubModel2,
		 *fPVectSubModel3, *fPVectSubModel4, *fPVectSubModel5, fVectInt1Value, fVectInt2Value;

	/* Boucle sur les sommets */

	for (i = 0; i < awl->maxNumberOfDetections; i++) 
	{

		if (fD[i] <= 0) { // Si aucune détection à la position i, il n'y a plus de détection
			break;
		}

		uiPos = (unsigned int) fD[i];

		fPSsub[0] = 0; // Valeur juste avant la position détectée
		fPSsub[1] = fPS[uiPos];
		fPSsub[2] = 0; // Valeur juste après la position détectée

		/* Calcul de PSpos */

		/** Calcul de PSint **/
		// On cherche deux points du pseudospectre en meme temps, celui juste avant
		// la position de l'echo et celui justre apres, d'ou les deux valeurs intermediaires

		/* j<0 */

		fPVectInt1 = fPSINT1;
		fPVectInt2 = fPSINT2;
		for (j = -iMODELHWF; j < 0; j++) {
			fVectInt1Value = 0;
			fVectInt2Value = 0;
			fPVectSubModel = fSUBMODELS + iMODELHWF - (nbdiags - 1); // Centre - (NBDIAG -1) du premier modèle du tableau
			fPVectSubModel2 = fSUBMODELS + iMODELHWF
					- (nbdiags - 1)+ (NBSUBMODELS-1)*LENGTHSUBMODELS; // Centre - (NBDIAG -1) du dernier modèle du tableau

			for (k = -(nbdiags - 1); k < nbdiags ; k++) {
				if (j - k > 0) {
					fVectInt1Value += *fPVectSubModel
							* fPN[uiPos + k + (j - k) * signalLength];
					fVectInt2Value += *fPVectSubModel2
							* fPN[uiPos + k + (j - k) * signalLength];
				} else if (k - j < nbdiags) {
					fVectInt1Value += *fPVectSubModel
							* fPN[uiPos + j + (k - j) * signalLength];
					fVectInt2Value += *fPVectSubModel2
							* fPN[uiPos + j + (k - j) * signalLength];
				}
				fPVectSubModel++;
				fPVectSubModel2++;
			}
			*fPVectInt1++ = fVectInt1Value;
			*fPVectInt2++ = fVectInt2Value;
		}

		/* j==0 */
		fVectInt1Value = 0;
		fVectInt2Value = 0;
		fPVectSubModel = fSUBMODELS + iMODELHWF - (nbdiags - 1); // Centre - (NBDIAG -1) du premier modèle du tableau
		fPVectSubModel2 = fSUBMODELS + iMODELHWF
				- (nbdiags - 1) + (NBSUBMODELS - 1)*LENGTHSUBMODELS; // Centre - (NBDIAG -1) du dernier modèle du tableau

		for (k = -(nbdiags - 1); k < 0; k++) {
			fVectInt1Value += *fPVectSubModel++ * fPN[uiPos + k + -k * signalLength];
			fVectInt2Value += *fPVectSubModel2++ * fPN[uiPos + k + -k * signalLength];
		}
		for (k = 0; k < nbdiags ; k++) {
			fVectInt1Value += *fPVectSubModel++ * fPN[uiPos + k * signalLength];
			fVectInt2Value += *fPVectSubModel2++ * fPN[uiPos + k * signalLength];
		}
		*fPVectInt1++ = fVectInt1Value;
		*fPVectInt2++ = fVectInt2Value;

		/* j>0 */
		for (j = 1; j <= iMODELHWF; j++) {
			fVectInt1Value = 0;
			fVectInt2Value = 0;
			fPVectSubModel = fSUBMODELS + iMODELHWF - (nbdiags - 1);
			fPVectSubModel2 = fSUBMODELS + iMODELHWF
					- (nbdiags - 1)+ (NBSUBMODELS-1)*LENGTHSUBMODELS;
			for (k = -(nbdiags - 1); k < nbdiags ; k++) {
				if (k - j >= 0) {
					fVectInt1Value += *fPVectSubModel
							* fPN[uiPos + j + (k - j) * signalLength];
					fVectInt2Value += *fPVectSubModel2
							* fPN[uiPos + j + (k - j) * signalLength];
				} else if (j - k < nbdiags) {
					fVectInt1Value += *fPVectSubModel
							* fPN[uiPos + k + (j - k) * signalLength];
					fVectInt2Value += *fPVectSubModel2
							* fPN[uiPos + k + (j - k) * signalLength];
				}
				fPVectSubModel++;
				fPVectSubModel2++;
			}
			*fPVectInt1++ = fVectInt1Value;
			*fPVectInt2++ = fVectInt2Value;
		}

		fPVectInt1 = fPSINT1;
		fPVectInt2 = fPSINT2;
		fPVectSubModel = fSUBMODELS;
		fPVectSubModel2 = fSUBMODELS + (NBSUBMODELS - 1) * LENGTHSUBMODELS;
		fVectInt1Value = 0;
		fVectInt2Value = 0;
		// Sommation et remultiplication par le modèle pour passer du vecteur intermédiaire
		// au véritable pseudospectre. Chaque vecteur intermédiaire représente un point du
		// pseudospectre.
		for (j = 0; j < LENGTHSUBMODELS; j++) {
			fVectInt1Value += *fPVectInt1++ * *fPVectSubModel++;
			fVectInt2Value += *fPVectInt2++ * *fPVectSubModel2++;
		}

		fPSsub[0] = fVectInt1Value;
		fPSsub[2] = fVectInt2Value;

		if (fPSsub[0] < 0) {
			fPSsub[0] = (float) abs((int) fPSsub[0]);
		}
		fPSsub[0] = 1.0 / fPSsub[0]; // Car dans le pseudospectre il faut toujours prendre l'inverse de la valeur

		if (fPSsub[2] < 0) {
			fPSsub[2] = (float) abs((int) fPSsub[2]);
		}
		fPSsub[2] = 1.0 / fPSsub[2];

		fDephInt[0] = 0.5; // Côté 'Pos'
		fDephInt[1] = 0;
		fDephInt[2] = -0.5; // Côté 'Neg'

		fSubStep = 0.5;
		uiModelsStep = 0.5 * (NBSUBMODELS - 1);
		uiModelPosition = uiModelsStep;

		while (fSubStep > PRECISION) {

			fSubStep = 0.5 * fSubStep; // 0.5 -> 0.25 -> 0.125 -> 0.0625 -> 0.03125
			uiModelsStep = 0.5 * uiModelsStep;

			/* Verifie de quel cote penche l'arbre */
			if (fPSsub[0] > fPSsub[2]) {
				iPosOverNeg = 1;
				fPSsub[2] = fPSsub[1];
				fDephInt[2] = fDephInt[1];
				fDephInt[1] = 0.5 * (fDephInt[0] + fDephInt[2]);
			} else {
				iPosOverNeg = 0;
				fPSsub[0] = fPSsub[1];
				fDephInt[0] = fDephInt[1];
				fDephInt[1] = 0.5 * (fDephInt[0] + fDephInt[2]);
			}

			/* Verifie si un modele existe, sinon en approxime un */
			if (fSubStep >= SUBMODELDEPTH) {
				if (iPosOverNeg) {
					uiModelPosition -= uiModelsStep;
				} else {
					uiModelPosition += uiModelsStep;
				}
				fPVectSubModel = fMODELTEMP + LENGTHSUBMODELS;
				fPVectSubModel2 = fMODELTEMP + 2 * LENGTHSUBMODELS;
				fPVectSubModel3 = fSUBMODELS + uiModelPosition * LENGTHSUBMODELS;
				fPVectSubModel4 = fSUBMODELS
						+ (uiModelPosition - uiModelsStep) * LENGTHSUBMODELS;
				fPVectSubModel5 = fSUBMODELS
						+ (uiModelPosition + uiModelsStep) * LENGTHSUBMODELS;
				for (j = 0; j < LENGTHSUBMODELS; j++) {
					*fPVectSubModel++ = *fPVectSubModel3++;          // ModelCentre
					fMODELTEMP[j] = *fPVectSubModel4++;            // ModelPos
					*fPVectSubModel2++ = *fPVectSubModel5++;  // ModelNeg
				}
			} else {

				if (iPosOverNeg) {
					fPVectSubModel = fMODELTEMP + 2 * LENGTHSUBMODELS;
					fPVectSubModel2 = fMODELTEMP + LENGTHSUBMODELS;
					for (j = 0; j < LENGTHSUBMODELS; j++) {
						*fPVectSubModel++ = *fPVectSubModel2++; // ModelNeg = ModelCentre
					}
				} else {
					fPVectSubModel = fMODELTEMP;
					fPVectSubModel2 = fMODELTEMP + LENGTHSUBMODELS;
					for (j = 0; j < LENGTHSUBMODELS; j++) {
						*fPVectSubModel++ = *fPVectSubModel2++;   // ModelPos = ModelCentre
					}
				}

				fPVectSubModel = fMODELTEMP + LENGTHSUBMODELS;
				fPVectSubModel2 = fMODELTEMP + 2 * LENGTHSUBMODELS;
				fPVectSubModel3 = fMODELTEMP;
				for (j = 0; j < LENGTHSUBMODELS; j++) {
					*fPVectSubModel++ = 0.5 * (*fPVectSubModel2++ + *fPVectSubModel3++); /* ModelCentre = 0.5*(ModelNeg + ModelPos) */
				}

			}

			/* Recalcul de PScentre */

			/** Calcul de PSint **/

			/* j<0 */

			fPVectInt1 = fPSINT1;
			for (j = -iMODELHWF; j < 0; j++) {
				fVectInt1Value = 0;
				fPVectSubModel = fMODELTEMP + iMODELHWF
						- (nbdiags - 1)+ LENGTHSUBMODELS;
				for (k = -(nbdiags - 1); k < nbdiags ; k++) {
					if (j - k > 0) {
						fVectInt1Value += *fPVectSubModel
								* fPN[uiPos + k + (j - k) * signalLength];
					} else if (k - j < nbdiags) {
						fVectInt1Value += *fPVectSubModel
								* fPN[uiPos + j + (k - j) * signalLength];
					}
					fPVectSubModel++;
				}
				*fPVectInt1++ = fVectInt1Value;
			}

			/* j==0 */
			fVectInt1Value = 0;
			fPVectSubModel = fMODELTEMP + iMODELHWF - (nbdiags - 1) + LENGTHSUBMODELS;
			for (k = -(nbdiags - 1); k < 0; k++) {
				fVectInt1Value += *fPVectSubModel++
						* fPN[uiPos + k + (-k) * signalLength];
			}
			for (k = 0; k < nbdiags ; k++) {
				fVectInt1Value += *fPVectSubModel++ * fPN[uiPos + k * signalLength];
			}
			*fPVectInt1++ = fVectInt1Value;

			/* j>0 */
			for (j = 1; j <= iMODELHWF; j++) {
				fVectInt1Value = 0;
				fPVectSubModel = fMODELTEMP + iMODELHWF
						- (nbdiags - 1)+ LENGTHSUBMODELS;
				for (k = -(nbdiags - 1); k < nbdiags ; k++) {
					if (k - j >= 0) {
						fVectInt1Value += *fPVectSubModel
								* fPN[uiPos + j + (k - j) * signalLength];
					} else if (j - k < nbdiags) {
						fVectInt1Value += *fPVectSubModel
								* fPN[uiPos + k + (j - k) * signalLength];
					}
					fPVectSubModel++;
				}
				*fPVectInt1++ = fVectInt1Value;
			}

			fPSsub[1] = 0;
			fPVectInt1 = fPSINT1;
			fPVectSubModel = fMODELTEMP + LENGTHSUBMODELS;
			for (j = 0; j < LENGTHSUBMODELS; j++) {
				fPSsub[1] += *fPVectInt1++ * *fPVectSubModel++;
			}
			if (fPSsub[1] < 0) {
				fPSsub[1] = (float) abs((int) fPSsub[1]);
			}
			fPSsub[1] = 1.0 / fPSsub[1];

		}    // Fin boucle arbre

		fPSMAX = 0;
		uiIDephMax = 0;
		for (j = 0; j < 2; j++) {
			if (fPSsub[j] > fPSMAX) {
				fPSMAX = fPSsub[j];
				uiIDephMax = j;
			}
		}

		fD[i] -= fDephInt[uiIDephMax];
		fD[i] += 1;

	}             // Fin boucle sur les sommets

	return;
}

