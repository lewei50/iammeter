
#include <mydsipweb.h>
#include <mymodbus.h>

/*
#include <lvgl.h>
#include <TFT_eSPI.h>
//#include <demos/lv_demos.h>
#include <TFT_Touch.h>
//#include <lv_demo.h>
//#include <examples/lv_examples.h>
*/
//static char *dtostrfd(double number, unsigned char prec, char *s);

#define LGFX_USE_V1
#include <Arduino.h>

#include <touchPanel.h>

int progress = 0;
const int BAR_LENGTH = 310;
const int BAR_HEIGHT = 5;

char *dtostrfd(double number, unsigned char prec, char *s)
{
    if ((isnan(number)) || (isinf(number)))
    { // Fix for JSON output (https://stackoverflow.com/questions/1423081/json-left-out-infinity-and-nan-json-status-in-ecmascript)
        strcpy(s, PSTR("null"));
        return s;
    }
    else
    {
        dtostrf(number, 1, prec, s);
        while (prec > 0)
        {
            if (s[strlen(s) - 1] == '0')
            {
                s[strlen(s) - 1] = 0;
            }
            else if (s[strlen(s) - 1] == '.')
            {
                s[strlen(s) - 1] = 0;
                break;
            }
            else
            {
                break;
            }
        }
        return s;
    }
}

// /*Change to your screen resolution*/
// static const uint16_t screenWidth  = 320;//480;
// static const uint16_t screenHeight = 240;//320;

// static lv_disp_draw_buf_t draw_buf;
// static lv_color_t buf[ screenWidth * screenHeight/4 ];

// TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */


LGFX tft;



static lv_obj_t * chart;
static lv_chart_series_t * chartSer; //表格内部数组
lv_coord_t dataArray[] = {}; //外部数组，显示到表格中
static lv_obj_t * cont;
static lv_obj_t * progressBar;

int metertype = 0;  //1:single, 3:Three
// int dataIndex = 0;
int currentPhase = 0;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    // tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
   uint16_t touchX, touchY;
   bool touched = tft.getTouch( &touchX, &touchY);
   if( !touched )
   {
      data->state = LV_INDEV_STATE_REL;
   }
   else
   {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;

      Serial.print( "Data x " );
      Serial.print( touchX );

      Serial.print( "/Data y " );
      Serial.println( touchY );
   }
}


//更新进度条
void updateProgress(int percent)
{
  progress = percent;
  lv_bar_set_value(progressBar, progress,LV_ANIM_OFF);
  // Serial.print( "progress " );
  // Serial.println( progress );
}

void addProgress()
{
  progress += 20;
  if(progress > 100)progress = 20;
  updateProgress(progress);
}


void showDatas(int phase)
{
  // lv_obj_t * cont = lv_obj_get_child(lv_scr_act(),1);
  //Serial.print("SHOW PHASE:");
  //Serial.println(phase);
  uint32_t i;
  lv_obj_t * labelMD;
  lv_obj_t * btnMD;
  for(i = 0; i < 6; i++) 
  {
    btnMD = lv_obj_get_child(cont,i);
    labelMD = lv_obj_get_child(btnMD,1);
    // Serial.println(wem3080t.Voltage_a);
    // Serial.println(wem3080t.Current_a);
    // Serial.println(wem3080t.Power_a);
    // Serial.println(wem3080t.ImportEnergy_a);
    // Serial.println(wem3080t.ExportGrid_a);
    // Serial.println(wem3080t.power_factor_a);
    // float Voltage_a ; //  0x0000  /100 V
    // float Current_a;  //  0x0001  /100 A
    // float Power_a;  //  0x0002  W
    // double ImportEnergy_a;  //  0x0004 /800 kWh
    // double ExportGrid_a;  //  0x0006 /800 kWh
    // float power_factor_a; //  0x0008 /1000 
    switch (phase)
    {
      case 0:
        if(i==0)      lv_label_set_text_fmt(labelMD, "%.1f", wem3080t.Voltage_a);
        else if(i==1) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.Current_a);
        else if(i==2) lv_label_set_text_fmt(labelMD, "%.0f", wem3080t.Power_a);
        else if(i==3) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.ImportEnergy_a);
        else if(i==4) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.ExportGrid_a);
        else if(i==5) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.power_factor_a);
        break;
      case 1:
        if(i==0)      lv_label_set_text_fmt(labelMD, "%.1f", wem3080t.Voltage_b);
        else if(i==1) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.Current_b);
        else if(i==2) lv_label_set_text_fmt(labelMD, "%.0f", wem3080t.Power_b);
        else if(i==3) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.ImportEnergy_b);
        else if(i==4) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.ExportGrid_b);
        else if(i==5) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.power_factor_b);
        break;
      case 2:
        if(i==0)      lv_label_set_text_fmt(labelMD, "%.1f", wem3080t.Voltage_c);
        else if(i==1) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.Current_c);
        else if(i==2) lv_label_set_text_fmt(labelMD, "%.0f", wem3080t.Power_c);
        else if(i==3) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.ImportEnergy_c);
        else if(i==4) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.ExportGrid_c);
        else if(i==5) lv_label_set_text_fmt(labelMD, "%.2f", wem3080t.power_factor_c);
        break;
    default:
      break;
    }
  }
  // if(dataIndex == 0)
  // {
  //   lv_chart_set_next_value(chart, chartSer, wem3080t.Voltage_a);
  //   Serial.print("wem3080t.Voltage_a");
  //   Serial.println(wem3080t.Voltage_a);
  // }
  // else if(dataIndex == 1) lv_chart_set_next_value(chart, chartSer, wem3080t.Current_a);
  // else if(dataIndex == 2) lv_chart_set_next_value(chart, chartSer, wem3080t.Power_a);
  // else if(dataIndex == 3) lv_chart_set_next_value(chart, chartSer, wem3080t.ImportEnergy_a);
  // else if(dataIndex == 4) lv_chart_set_next_value(chart, chartSer, wem3080t.ExportGrid_a);
  // else if(dataIndex == 5) lv_chart_set_next_value(chart, chartSer, wem3080t.power_factor_a);
}

static void chart_click_event_cb(lv_event_t * e)
{
    // lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_move_background(chart);
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
      // dataIndex = lv_obj_get_index(btn);
      // switch(dataIndex)
      // {
      //   case 0:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 225, 227);
      //   case 1:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 10);
      //   case 2:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -50, 50);
      //   case 3:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);
      //   case 4:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);
      //   case 5:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1);
      //   default:
      //     lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 500);
      // }
        
      //刷新数据显示
      // lv_obj_move_foreground(chart);
      // lv_chart_set_all_value(chart, chartSer, LV_CHART_POINT_NONE);
    }
}

void createProgressBar(void)
{
  progressBar = lv_bar_create(lv_scr_act());
  lv_obj_set_size(progressBar, BAR_LENGTH, BAR_HEIGHT);
  lv_obj_align(progressBar, NULL, LV_ALIGN_BOTTOM_MID, 0);
  lv_bar_set_value(progressBar, 0, LV_ANIM_ON);
}


static void change_phase_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    bool prev = id == 0 ? true : false;
    bool next = id == 4 ? true : false;
    if(prev || next) {
        /*Find the checked button*/
        uint32_t i;
        for(i = 1; i < 5; i++) {
            if(lv_btnmatrix_has_btn_ctrl(obj, i, LV_BTNMATRIX_CTRL_CHECKED)) break;
        }

        if(prev && i > 1) i--;
        else if(next && i < 3) i++;

        lv_btnmatrix_set_btn_ctrl(obj, i, LV_BTNMATRIX_CTRL_CHECKED);
        
        Serial.print("id:");
        Serial.println(i);
        currentPhase = i-1;
        showDatas(i-1);
    }
    else if(!prev && !next)
    {
      currentPhase = id-1;
      showDatas(id-1);
    }
}

/**
 * Basic example to create a "Hello world" label
 */
void createButtonList(void)
{
  
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    // lv_obj_t * label = lv_label_create(lv_scr_act());
    //lv_label_set_text(label, "Hello world");
    
    float voltage = wem3080t.Voltage_a;
    float current = wem3080t.Current_a;
    float power = wem3080t.power_factor_a;
    float energyf = float(wem3080t.ImportEnergy_a);
    float energyt = float(wem3080t.ExportGrid_a);
    float frequency = wem3080t.Frequency;
    float pf = wem3080t.power_factor_a;

    char voltage_chr[16];
    char current_chr[16];
    char power_chr[16];
    char energyf_chr[16];
    char energyt_chr[16];
    char frequency_chr[16];
    char pf_chr[16];

  static lv_coord_t col_dsc[] = {91, 91, 91, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {80, 80, LV_GRID_TEMPLATE_LAST};

  /*Create a container with grid*/
  // lv_obj_t * cont = lv_obj_create(lv_scr_act());
  cont = lv_obj_create(lv_scr_act());
  lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
  lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
  lv_obj_set_size(cont, 320, 240);
  lv_obj_center(cont);
  lv_obj_set_layout(cont, LV_LAYOUT_GRID);

  lv_obj_t * label;
  lv_obj_t * obj;
  lv_obj_t * ta;

  uint32_t i;
  for(i = 0; i < 6; i++) {
      uint8_t col = i % 3;
      uint8_t row = i / 3;

      obj = lv_btn_create(cont);
      // 绑定按键点击事件
      // lv_obj_add_event_cb(obj, btn_event_cb, LV_EVENT_ALL, NULL); 
      // lv_obj_set_size(obj,100,100);
      /*Stretch the cell horizontally and vertically too
        *Set span to 1 to make the cell 1 column/row sized*/
      lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
                            LV_GRID_ALIGN_STRETCH, row, 1);

      label = lv_label_create(obj);
      // lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
      // lv_obj_set_width(label, 130);
      static lv_style_t style_shadow;
      lv_label_set_recolor(label, true); 
      if(i==0)      lv_label_set_text(label, "#0000ff Voltage#");
      else if(i==1) lv_label_set_text(label, "#0000ff Current#");
      else if(i==2) lv_label_set_text(label, "#0000ff Power#");
      else if(i==3) lv_label_set_text(label, "#0000ff ImptEnergy#");
      else if(i==4) lv_label_set_text(label, "#0000ff ExportGrid#");
      else if(i==5) lv_label_set_text(label, "#0000ff PF#");
      // lv_obj_center(label);
      ta = lv_label_create(obj);
      lv_label_set_text_fmt(ta, "data");
      lv_obj_center(ta);
  }

  static lv_style_t style_bg;
  lv_style_init(&style_bg);
  lv_style_set_pad_all(&style_bg, 0);
  lv_style_set_pad_gap(&style_bg, 0);
  lv_style_set_clip_corner(&style_bg, true);
  lv_style_set_radius(&style_bg, LV_RADIUS_CIRCLE);
  lv_style_set_border_width(&style_bg, 0);

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_radius(&style_btn, 0);
  lv_style_set_border_width(&style_btn, 1);
  lv_style_set_border_opa(&style_btn, LV_OPA_50);
  lv_style_set_border_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
  lv_style_set_border_side(&style_btn, LV_BORDER_SIDE_INTERNAL);
  lv_style_set_radius(&style_btn, 0);

  static const char * map[] = {LV_SYMBOL_LEFT, "A", "B", "C", LV_SYMBOL_RIGHT, ""};

  lv_obj_t * btnm = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm, map);
  lv_obj_add_style(btnm, &style_bg, 0);
  lv_obj_add_style(btnm, &style_btn, LV_PART_ITEMS);
  lv_obj_add_event_cb(btnm, change_phase_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_set_size(btnm, 225, 35);

  /*Allow selecting on one number at time*/
  lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_clear_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_clear_btn_ctrl(btnm, 4, LV_BTNMATRIX_CTRL_CHECKABLE);

  lv_btnmatrix_set_one_checked(btnm, true);
  lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKED);
  lv_obj_set_align(btnm, LV_ALIGN_BOTTOM_MID);

  
}


void createChart(void)
{
    /*Create a chart*/
    // lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_add_event_cb(chart, chart_click_event_cb, LV_EVENT_CLICKED, NULL); 
    lv_obj_set_size(chart, 200, 150);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    lv_chart_set_point_count(chart, 10);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

    /*Add two data series*/
    chartSer = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    // lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_SECONDARY_Y);

    /*Set the next points on 'ser1'*/
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 10);
    // lv_chart_set_next_value(chart, chartSer, 30);
    // lv_chart_set_next_value(chart, chartSer, 70);
    // lv_chart_set_next_value(chart, chartSer, 90);
    // lv_chart_set_next_value(chart, chartSer, 80);
    // lv_chart_set_next_value(chart, chartSer, 70);

    lv_chart_set_all_value(chart, chartSer, LV_CHART_POINT_NONE);
    // lv_chart_set_ext_y_array(chart, chartSer, dataArray);

    /*Directly set points on 'ser2'*/
    // ser2->y_points[0] = 90;
    // ser2->y_points[1] = 70;
    // ser2->y_points[2] = 65;
    // ser2->y_points[3] = 65;
    // ser2->y_points[4] = 65;
    // ser2->y_points[5] = 65;
    // ser2->y_points[6] = 65;
    // ser2->y_points[7] = 65;
    // ser2->y_points[8] = 65;
    // ser2->y_points[9] = 65;

    lv_chart_refresh(chart); /*Required after direct set*/
}

void webinit()
{
  MeterGroup.addItem(&MeterIpParam);
  MeterGroup.addItem(&metertypeParam);

  iotWebConf.setStatusPin(STATUS_PIN);
  //iotWebConf.setConfigPin(CONFIG_PIN);

  iotWebConf.setHtmlFormatProvider(&optionalGroupHtmlFormatProvider);
  iotWebConf.addParameterGroup(&MeterGroup);
  iotWebConf.setConfigSavedCallback(&configSaved);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  // -- Define how to handle updateServer calls.
  iotWebConf.setupUpdateServer(
    [](const char* updatePath) { httpUpdater.setup(&server, updatePath); },
    [](const char* userName, char* password) { httpUpdater.updateCredentials(userName, password); });

  // -- Initializing the configuration.
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");
}
void mylvglinit()
{

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println( LVGL_Arduino );
  Serial.println( "I am LVGL_Arduino" );

  // lv_init();
  tft.begin();        
  tft.setRotation(3);
  tft.setBrightness(20);
  uint16_t calData[] = { 239, 3926, 233, 265, 3856, 3896, 3714, 308};
  tft.setTouchCalibrate(calData);
  //touch_calibrate();
  lv_init();
  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * 10 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  #if LV_USE_LOG != 0
      lv_log_register_print_cb( my_print ); /* register print function for debugging */
  #endif

  #if 0
      /* Create simple label */
      lv_obj_t *label = lv_label_create( lv_scr_act() );
      lv_label_set_text( label, LVGL_Arduino.c_str() );
      lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
  #else
    createChart();
    createButtonList();
    createProgressBar();
    updateProgress(5);
  #endif
  Serial.println( "Setup done" );
}
void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  webinit();
  mb.client();
  mylvglinit();
}

uint16_t res = 0;
long show = millis();
void getdataloop()
{
      parseBytes(MeterIp, '.', ip, 4, 10);
      IPAddress remote(ip); 

      if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
        //mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
        metertype = atof(metertypeParamValue);
        if(metertype == 3){
          mb.readHreg(remote, REG, read_datas, wem3080t.quantity, NULL);
          wem3080t.Voltage_a = float(read_datas[0])/100;
          wem3080t.Current_a = float(read_datas[1])/100;
          wem3080t.Power_a = float(read_datas[2]<<16|read_datas[3]);
          wem3080t.ImportEnergy_a = float(read_datas[4]<<16|read_datas[5])/800;
          wem3080t.ExportGrid_a = float(read_datas[6]<<16|read_datas[7])/800;
          wem3080t.power_factor_a = float(read_datas[8])/1000;

          wem3080t.Voltage_b = float(read_datas[10])/100;
          wem3080t.Current_b = float(read_datas[11])/100;
          wem3080t.Power_b = float(read_datas[12]<<16|read_datas[13]);
          wem3080t.ImportEnergy_b = float(read_datas[14]<<16|read_datas[15])/800;
          wem3080t.ExportGrid_b = float(read_datas[16]<<16|read_datas[17])/800;
          wem3080t.power_factor_b = float(read_datas[18])/1000;

          wem3080t.Voltage_c = float(read_datas[20])/100;
          wem3080t.Current_c = float(read_datas[21])/100;
          wem3080t.Power_c = float(read_datas[22]<<16|read_datas[23]);
          wem3080t.ImportEnergy_c = float(read_datas[24]<<16|read_datas[25])/800;
          wem3080t.ExportGrid_c = float(read_datas[26]<<16|read_datas[27])/800;
          wem3080t.power_factor_c = float(read_datas[28])/1000;

          wem3080t.Frequency = float(read_datas[30])/100; 
          wem3080t.Power_s = float(read_datas[32]<<16|read_datas[33]);
          wem3080t.ImportEnergy_s = float(read_datas[34]<<16|read_datas[35])/800;
          wem3080t.ExportGrid_s = float(read_datas[36]<<16|read_datas[37])/800;

          addProgress();
        }else{
          mb.readHreg(remote, REG, read_datas, wem3080t.quantitysingle, NULL);
          wem3080t.Voltage_a = float(read_datas[0])/100;
          wem3080t.Current_a = float(read_datas[1])/100;
          wem3080t.Power_a = float(read_datas[2]<<16|read_datas[3]);
          wem3080t.ImportEnergy_a = float(read_datas[4]<<16|read_datas[5])/3200;
          wem3080t.ExportGrid_a = float(read_datas[6]<<16|read_datas[7])/3200;
        }
        /*
        Serial.println("------");
        Serial.println(wem3080t.Voltage_a);
        Serial.println(wem3080t.Current_a);
        Serial.println(wem3080t.Power_a);
        Serial.println(wem3080t.ImportEnergy_a);
        Serial.println(wem3080t.ExportGrid_a);
        Serial.println(wem3080t.power_factor_a);

        Serial.println(wem3080t.Frequency);
        Serial.println(wem3080t.Power_s);
        Serial.println(wem3080t.ImportEnergy_s);
        Serial.println(wem3080t.ExportGrid_s); 
        */  
        showDatas(currentPhase); 

      } else {
        mb.connect(remote);           // Try to connect if no connection
        updateProgress(20);
      }
}
void loop()
{
  if (WiFi.isConnected())
  {
    mb.task();                      // Common local Modbus task
    
    if (millis() - show > refreshTime) {  // Display Slave register value one time per second (with default settings)
      getdataloop();
      //showDatas(currentPhase);
      show = millis();
    }
  }
  lv_timer_handler(); 
  delay(5);
  //  vTaskDelay(5);

  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();

}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IAMMETER Display</title></head><body><div>Status page of ";
  s += iotWebConf.getThingName();
  s += ".</div>";

  if (MeterGroup.isActive())
  {
    s += "<div>IAMMETER Display Setting</div>";
    s += "<ul>";
    s += "<li>IAMMETER IP: ";
    s += MeterIp;
    s += "<li>MeterType: ";
    s += atoi(metertypeParamValue);
    s += "</ul>";
  }
  else
  {
    s += "<div>IAMMETER IP not defined</div>";
  }
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}
void wifiConnected()
{
  Serial.println("WiFi was connected.");
}
void configSaved()
{
  Serial.println("Configuration was updated.");
}
void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}