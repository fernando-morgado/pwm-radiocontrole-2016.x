#include <xc.h>
#include "test.h"
#include "pwm.h"

#define PWM_NOMBRE_DE_CANAUX 2
#define PWM_ESPACEMENT 6

/** Numéro du canal. 0=CCP1 (PWM1) et 1=CCP3 (PWM3)*/
static unsigned char numeroCanal;

/** Valeurs des captures canaux. */
static unsigned char valeurCaptureCanal[2];

/** Valeurs des canaux. */
static unsigned char valeurCanal[2];

/** Valeur pour totaliser le nombre d'espacement pour le PWM.*/
/** Pour être compatible avec la norme radio contrôle.*/
static unsigned char nbEspacement = 0;

/**
 * Convertit une valeur signée générique vers une valeur directement
 * utilisable pour la génération PWM.
 * @param valeur Une valeur entre 0 et 255.
 * @return Une valeur entre 62 et 125.
 */
unsigned char pwmConversion(unsigned char valeurGenerique) {
    // À implémenter...
    return (valeurGenerique / 4) + 62; // 0 à 255 devient 62 à 125.
}

/**
 * Indique sur quel canal la valeur doit changer.
 * @param canal Le numéro de canal.
 */
void pwmPrepareValeur(unsigned char canal) {
    // À implémenter...
    if (canal){
        numeroCanal = 1; // Pour CCP3(=ECCP3). Générateur PWM 3.
    }
    else {
        numeroCanal = 0; // Pour CCP1(=ECCP1). Générateur PWM 1.
    }
}

/**
 * Établit la valeur du canal spécifié par {@link #pwmPrepareValeur}.
 * @param valeur La valeur du canal.
 */
void pwmEtablitValeur(unsigned char valeur) {
    // À implémenter...
    unsigned char valeurConvertit;
    
    valeurConvertit = pwmConversion(valeur); // Conversion 0/255 => 62/125.
    if (numeroCanal){ // Valeur du conv. A/D pour le rapport cyclique PWM.
        valeurCanal[1] = valeurConvertit;
    }
    else { // Valeur du convertisseur A/D pour le rapport cyclique PWM.
        valeurCanal[0] = valeurConvertit;
    }
}

/**
 * Rend la valeur PWM correspondante au canal.
 * @param canal Le numéro de canal.
 * @return La valeur PWM correspondante au canal.
 */
unsigned char pwmValeur(unsigned char canal) {
    // À implémenter...
    if (canal) {
        return valeurCanal[1]; // Rapport cyclique pour le PWM 3
    }
    else {
        return valeurCanal[0]; // Rapport cyclique pour le PWM 1
    }
}

/**
 * Indique si il est temps d'émettre une pulsation PWM.
 * Sert à espacer les pulsation PWM pour les rendre compatibles
 * avec la norme de radio contrôle.
 * @return 255 si il est temps d'émettre une pulse. 0 autrement.
 */
unsigned char pwmEspacement() {
    // À implémenter...
    // Si le nb d'espacement est plus petit que le nb total il retourne 0.
    if (nbEspacement < PWM_ESPACEMENT) { 
        nbEspacement++;
        return 0;
    }
    // Sinon il retourne 255.
    else {
        return 255;
    }
}

/**
 * Démarre une capture sur le canal indiqué.
 * @param canal Numéro du canal.
 * @param instant Instant de démarrage de la capture.
 */
void pwmDemarreCapture(unsigned char canal, unsigned int instant) {
    // À implémenter...
    if (canal){
        valeurCaptureCanal[1] = instant; // instant => valeurCaptureCanal[1].
    }
    else {
        valeurCaptureCanal[0] = instant; // instant => valeurCaptureCanal[0].
    }
}

/**
 * Complète une capture PWM, et met à jour le canal indiqué.
 * @param canal Le numéro de canal.
 * @param instant L'instant de finalisation de la capture.
 */
void pwmCompleteCapture(unsigned char canal, unsigned int instant) {
    // À implémenter...
    if (canal){
        valeurCanal[1] = (instant - valeurCaptureCanal[1]); // Dif. entre les 2.
    }
    else {
        valeurCanal[0] = (instant - valeurCaptureCanal[0]); // Dif. entre les 2.
    }
}

/**
 * Réinitialise le système PWM.
 */
void pwmReinitialise() {
    // À implémenter...
    numeroCanal = 0;
    valeurCanal[1] = 0;
    valeurCanal[0] = 0;
    nbEspacement = 0;

}

#ifdef TEST
void testConversionPwm() {
    testeEgaliteEntiers("PWMC001", pwmConversion(  0),  62);
    testeEgaliteEntiers("PWMC002", pwmConversion(  4),  63);

    testeEgaliteEntiers("PWMC003", pwmConversion(126),  93);
    
    testeEgaliteEntiers("PWMC004", pwmConversion(127),  93);
    testeEgaliteEntiers("PWMC005", pwmConversion(128),  94);
    testeEgaliteEntiers("PWMC006", pwmConversion(129),  94);
    
    testeEgaliteEntiers("PWMC007", pwmConversion(132),  95);

    testeEgaliteEntiers("PWMC008", pwmConversion(251), 124);
    testeEgaliteEntiers("PWMC009", pwmConversion(255), 125);
}
void testEtablitEtLitValeurPwm() {
    pwmReinitialise();
    
    pwmPrepareValeur(0);
    pwmEtablitValeur(80);
    testeEgaliteEntiers("PWMV01", pwmValeur(0), pwmConversion(80));
    testeEgaliteEntiers("PWMV02", pwmValeur(1), 0);

    pwmPrepareValeur(1);
    pwmEtablitValeur(180);
    testeEgaliteEntiers("PWMV03", pwmValeur(0), pwmConversion( 80));
    testeEgaliteEntiers("PWMV04", pwmValeur(1), pwmConversion(180));
}
void testEspacementPwm() {
    unsigned char n;
    
    pwmReinitialise();

    for (n = 0; n < PWM_ESPACEMENT; n++) {
        testeEgaliteEntiers("PWME00", pwmEspacement(), 0);
    }

    testeEgaliteEntiers("PWME01", pwmEspacement(), 255); 
}
void testCapturePwm() {
    
    pwmDemarreCapture(0, 0);
    pwmCompleteCapture(0, 80);
    
    pwmDemarreCapture(1, 10000);
    pwmCompleteCapture(1, 10090);

    testeEgaliteEntiers("PWMC01a", pwmValeur(0), 80);
    testeEgaliteEntiers("PWMC01b", pwmValeur(1), 90);
    
    pwmDemarreCapture(0, 65526);
    pwmCompleteCapture(0, 80);
    
    pwmDemarreCapture(1, 65516);
    pwmCompleteCapture(1, 80);
    
    testeEgaliteEntiers("PWMC02a", pwmValeur(0), 90);
    testeEgaliteEntiers("PWMC02b", pwmValeur(1), 100);    
}
void testPwm() {    
    testConversionPwm();
    testEtablitEtLitValeurPwm();
    testEspacementPwm();
    testCapturePwm();
}

#endif