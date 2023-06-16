
#include <lvgl.h>
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
lgfx::Panel_ILI9341     _panel_instance;
lgfx::Bus_SPI       _bus_instance;   // SPI bus instance // SPIバスのインスタンス
lgfx::Light_PWM     _light_instance;
lgfx::Touch_XPT2046     _touch_instance;

public:
  LGFX(void)
  {
    { // バス制御の設定を行います。
      auto cfg = _bus_instance.config();    // Gets the structure for bus configuration. // バス設定用の構造体を取得します。
      cfg.spi_host = HSPI_HOST;     // Select the SPI to use // 使用するSPIを選択  (VSPI_HOST or HSPI_HOST)
      cfg.spi_mode = 0;             // Set SPI communication mode (0 ~ 3) // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 55000000;    // SPI clock at the time of transmission (up to 80MHz, rounded to the value obtained by dividing 80MHz by an integer) // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      cfg.freq_read  = 20000000;    // SPI clock when receiving // 受信時のSPIクロック
      cfg.spi_3wire  = false;       // Set true when receiving with MOSI pin // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock   = true;        // Set to true when using transaction lock // トランザクションロックを使用する場合はtrueを設定
      cfg.dma_channel = 1;          // Set the DMA channel (1 or 2. 0=disable) // 使用するDMAチャンネルを設定 (0=DMA不使用)
      cfg.pin_sclk = 14;            // Set SPI SCLK pin number // SPIのSCLKピン番号を設定
      cfg.pin_mosi = 13;            // Set SPI MOSI pin number // SPIのMOSIピン番号を設定
      cfg.pin_miso = 12;            // Set SPI MISO pin number (-1 = disable) // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc   = 2;             // Set SPI D / C pin number (-1 = disable) // SPIのD/Cピン番号を設定  (-1 = disable)

      _bus_instance.config(cfg);    // The set value is reflected on the bus. // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance);      // Set the bus on the panel. // バスをパネルにセットします。
    }

    { // Set the display panel control.//表示パネル制御の設定を行います。
      auto cfg = _panel_instance.config();    // Gets the structure for display panel settings.// 表示パネル設定用の構造体を取得します。
      cfg.pin_cs           =    15;  // Pin number to which CS is connected (-1 = disable) // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst          =    -1;  // Pin number to which RST is connected (-1 = disable) // RSTが接続されているピン番号  (-1 = disable)
      cfg.pin_busy         =    -1;  // Pin number to which BUSY is connected (-1 = disable) // BUSYが接続されているピン番号 (-1 = disable)
      cfg.memory_width     =   240;  // Maximum width supported by driver IC // ドライバICがサポートしている最大の幅
      cfg.memory_height    =   320;  // Maximum height supported by driver IC // ドライバICがサポートしている最大の高さ
      cfg.panel_width      =   240;  // Actually displayable width // 実際に表示可能な幅
      cfg.panel_height     =   320;  // Actually displayable height // 実際に表示可能な高さ
      cfg.offset_x         =     0;  // Amount of X-direction offset of the panel // パネルのX方向オフセット量
      cfg.offset_y         =     0;  // Amount of Y-direction offset of the panel // パネルのY方向オフセット量
      cfg.offset_rotation  =     0;  // Offset of values in the direction of rotation 0 ~ 7 (4 ~ 7 are upside down) // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel =     8;  // Number of dummy read bits before pixel reading // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits  =     1;  // Number of bits of dummy read before reading data other than pixels // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable         =  true;  // Set to true if data can be read // データ読出しが可能な場合 trueに設定
      cfg.invert           = false;  // Set to true if the light and darkness of the panel is reversed // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order        = false;  // Set to true if the red and blue of the panel are swapped // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit       = false;  // Set to true for panels that send data length in 16-bit units // データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared       =  true;  // If the bus is shared with the SD card, set to true (bus control is performed with drawJpgFile etc.) // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }
    
    { // Set the backlight control. (Delete if not needed // バックライト制御の設定を行います。（必要なければ削除）
      auto cfg = _light_instance.config();    // Gets the structure for the backlight setting. // バックライト設定用の構造体を取得します。

      cfg.pin_bl = 21;              // Pin number to which the backlight is connected // バックライトが接続されているピン番号
      cfg.invert = false;           // True if you want to invert the brightness of the backlight // バックライトの輝度を反転させる場合 true
      cfg.freq   = 44100;           // Backlight PWM frequency // バックライトのPWM周波数
      cfg.pwm_channel = 7;          // PWM channel number to use // 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // Set the backlight on the panel. // バックライトをパネルにセットします。
    }

    { // Set the touch screen control. (Delete if not needed) // タッチスクリーン制御の設定を行います。（必要なければ削除）
      auto cfg = _touch_instance.config();
      cfg.x_min      = 0;    // Minimum X value (raw value) obtained from touch screen // タッチスクリーンから得られる最小のX値(生の値)
      cfg.x_max      = 239;  // Maximum X value (raw value) obtained from the touch screen // タッチスクリーンから得られる最大のX値(生の値)
      cfg.y_min      = 0;    // Minimum Y value (raw value) obtained from touch screen // タッチスクリーンから得られる最小のY値(生の値)
      cfg.y_max      = 319;  // Maximum Y value (raw value) obtained from the touch screen // タッチスクリーンから得られる最大のY値(生の値)
      cfg.pin_int    = 36;   // Pin number to which INT is connected // INTが接続されているピン番号
      cfg.bus_shared = true; // Set to true if you are using the same bus as the screen // 画面と共通のバスを使用している場合 trueを設定
      cfg.offset_rotation = 0;// Adjustment when the display and touch orientation do not match Set with a value from 0 to 7 // 表示とタッチの向きのが一致しない場合の調整 0~7の値で設定
      cfg.spi_host = VSPI_HOST;// Select the SPI to use (HSPI_HOST or VSPI_HOST) // 使用するSPIを選択 (HSPI_HOST or VSPI_HOST)
      cfg.freq = 2500000;     // Set SPI clock // SPIクロックを設定
      cfg.pin_sclk = 25;     // Pin number to which SCLK is connected // SCLKが接続されているピン番号
      cfg.pin_mosi = 32;     // Pin number to which MOSI is connected // MOSIが接続されているピン番号
      cfg.pin_miso = 39;     // Pin number to which MISO is connected // MISOが接続されているピン番号
      cfg.pin_cs   = 33;     // Pin number to which CS is connected // CSが接続されているピン番号
      
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  // Set the touch screen on the panel. // タッチスクリーンをパネルにセットします。
    }
    setPanel(&_panel_instance); // Set the panel to be used. // 使用するパネルをセットします。
  }
};


/*Change to your screen resolution*/
static const uint32_t screenWidth  = 320;
static const uint32_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

