#ifndef __NBM_EEPROM_H__
#define __NBM_EEPROM_H__

/* Slot no is the number printed in the front panel, not slot array index used in software*/

int nbm_read_chassis_module_sysinfo(int slotno,struct module_info_s *module) ;
int nbm_read_mainboard_sysinfo(struct product_s *product_info, struct module_info_s *module);
int nbm_read_backplane_sysinfo(struct product_s *product_info) ;
#endif
