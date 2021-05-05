// Define IO pins
#define DRF4463_SDN 0
#define DRF4463_nIRQ 1
#define DRF4463_SDO 2
#define DRF4463_SCLK 3
#define DRF4463_SDI 4
#define DRF4463_nSEL 13

bool setupRadio(void);
void getRadioStatus(void);
void setModeRx(void);