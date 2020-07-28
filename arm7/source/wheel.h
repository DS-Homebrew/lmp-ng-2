#define CENTER_X SCREEN_WIDTH/2
#define CENTER_Y SCREEN_HEIGHT/2

#define MENU_X 0
#define MENU_Y -60
#define FORWARD_X 60
#define FORWARD_Y 0
#define REWIND_X -60
#define REWIND_Y 0
#define PLAY_X 0
#define PLAY_Y 60

#define WHEEL_RIGHT        BIT(0)
#define WHEEL_LEFT         BIT(1)
#define WHEEL_MENU         BIT(2)
#define WHEEL_PLAY         BIT(3)
#define WHEEL_FORWARD      BIT(4)
#define WHEEL_REWIND       BIT(5)
#define WHEEL_CENTER       BIT(6)
#define WHEEL_MENU_HELD    BIT(7)
#define WHEEL_PLAY_HELD    BIT(8)
#define WHEEL_FORWARD_HELD BIT(9)
#define WHEEL_REWIND_HELD  BIT(10)
#define WHEEL_CENTER_HELD  BIT(11)

void scanwheel();
u16 readwheel(void);
