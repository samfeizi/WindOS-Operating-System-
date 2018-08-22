
#include "PIT.h"


/*
 * PIT_init
 *   DESCRIPTION: Sets the PIT frequency
 *   INPUTS: PIT_frequency
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes the PIT frequency
 */
void PIT_init(uint32_t PIT_frequency){
  uint32_t divider = HIGHEST_FREQ / PIT_frequency;

  if(PIT_frequency < LOW_FREQ || PIT_frequency > HIGH_FREQ){
    printf("Yaa, that frequency ain't gonna work");
    return;
  }

  outb(THIRTY_SIX, MODE_COMMAND_REGISTER);

  disable_irq(PIT_IRQ);

  uint8_t l = (uint8_t) ((divider) & EIGHT_BIT_MASK);
  uint8_t h = (uint8_t) ((divider >> 8) & EIGHT_BIT_MASK);

  outb(l, CHAN_ZERO);
  outb(h, CHAN_ZERO);

  enable_irq(PIT_IRQ);

  return;

}

/*
 * PIT_handler
 *   DESCRIPTION: Calls the scheudler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: a lot of stuff, modifies stack, paging, screen and member variables
 */
void
PIT_handler(){

  // printf("1");

  send_eoi(PIT_IRQ);

  // scheduler();

}
