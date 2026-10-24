#ifndef _API_GUI_H
#define _API_GUI_H

typedef struct rect_t_ {
    s16 x;
    s16 y;
    s16 wid;
    s16 hei;
} rect_t;

typedef struct point_t_ {
    s16 x;
    s16 y;
} point_t;

typedef struct area_t_ {
    s16 wid;
    s16 hei;
} area_t;

typedef void widget_t;
typedef void widget_page_t;
typedef void widget_image_t;        
typedef void widget_text_t;
typedef void widget_rect_t;

//=== 常用颜色，非常用颜色可以用make_color函数来生成 ===
#define COLOR_WHITE             0xFFFF
#define COLOR_BLACK             0
#define COLOR_RED               0x00F8
#define COLOR_GREEN             0xE007
#define COLOR_BLUE              0x1F00
#define COLOR_YELLOW            0xE0FF
#define COLOR_MAGENTA           0x1FF8
#define COLOR_CYAN              0xFF07
#define COLOR_GRAY              0x1084
#define COLOR_DARKGRAY          0x55AD
#define COLOR_DIMGRAY           0x4D6B

//=== GUI ===
//GUI初始化参数
typedef struct gui_init_param_t_ {
    u16 screen_width;               //屏幕宽
    u16 screen_height;              //屏幕高
    u8 *element_buf;                //element与widget缓存
    u8 *widget_buf;
    u16 element_buf_size;
    u16 widget_buf_size;
    u8 *temp_buf;                   //中间计算缓存
    u32 temp_buf_size;              //中间计算缓存大小
    u8 *lines_buf;                  //推屏缓存(双份)
    u32 lines_buf_size;             //推屏缓存大小
    u16 lines_count;                //每次推屏的行数
    u16 maxsize_rlebuf;             //RLE解码缓存
    u32 font_res_addr;              //字库资源地址
    u16 max_font_size;
    u8 font_wspace;                 //字的间距
    u8 font_hspace;
    u8 rsvd[20];
} gui_init_param_t;

void os_gui_init(const gui_init_param_t *param);        //GUI初始化
void os_gui_draw(void);                                 //TE信号起来后，GUI开始绘制图形
void gui_process(void);                                 //主循环调用，更新Widget绘制
void gui_widget_refresh(void);                          //刷新widget
void os_gui_draw_w4_done(void);                         //等待当前帧刷完
area_t gui_image_get_size(u32 res_addr);                //获取图像尺寸
area_t widget_image_get_size(widget_image_t *img);		//根据控件获取图像尺寸


/**
 * @brief 生成一个RGB565的颜色值
 * @param[in] r : #ff0000  红色饱和度(0 ~ 0xff)
 * @param[in] g : #00ff00  绿色饱和度(0 ~ 0xff)
 * @param[in] b : #0000ff  蓝色饱和度(0 ~ 0xff)
 * @return RGB565的颜色值
 **/
u16 make_color(u8 r, u8 g, u8 b);


//============= Widget Common ==============//

/**
 * @brief 创建控件池(最多允许创建两个)，并创建首个widget_page
 * @param[in] flag_top : 是否在顶层
 * @return page
 **/
widget_page_t *widget_pool_create(bool flag_top);

/**
 * @brief 清除一个控件池
 * @param[in] widget : 首个widget_page
 * @return 无
 **/
void widget_pool_clear(widget_page_t *widget);

/**
 * @brief 获取第一个Widget，配合widget_get_next来遍历所有Widget
 * @param[in] 无
 * @return 无
 **/
void *widget_get_head(void);

/**
 * @brief 根据当前widget获取下一个widget
 * @param[in] widget:当前的widget
 * @return next widget
 **/
widget_t *widget_get_next(const widget_t *widget);

/**
 * @brief 获取Widget的父Page
 * @param[in] widget:当前的widget
 * @return 当前widget的widget_page
 **/
widget_page_t *widget_get_parent(const widget_t *widget);

/**
 * @brief 设置Widget是否按中心点来设置坐标
 * @param[in] widget:需要设置的控件
 * @param[in] align_center:true:按中心对齐,false：按左上角对齐，default:true
 * @return 无
 **/
void widget_set_align_center(widget_t *widget, bool align_center);

/**
 * @brief 设置Widget是否按中心点来设置坐标
 * @param[in] widget:需要设置的控件
 * @param[in] align_center:true:按中心对齐,false：按左上角对齐，default:true
 * @return 无
 **/
bool widget_get_align_center(widget_t *widget);

/**
 * @brief 设置Widget坐标
 * @param[in] widget:需要设置的控件
 * @param[in] x:以屏幕左上角为原点，控件的中心点或左上角的x坐标
 * @param[in] y:以屏幕左上角为原点，控件的中心点或左上角的y坐标
 * @return 无
 **/
void widget_set_pos(widget_t *widget, s16 x, s16 y);

/**
 * @brief 设置Widget大小
 * @param[in] widget:需要设置的控件
 * @param[in] width:宽度
 * @param[in] height:高度
 * @return 无
 **/
void widget_set_size(widget_t *widget, s16 width, s16 height);

/**
 * @brief 设置Widget坐标及大小
 * @param[in] widget:需要设置的控件
 * @param[in] x:以屏幕左上角为原点，控件的中心点或左上角的x坐标
 * @param[in] y:以屏幕左上角为原点，控件的中心点或左上角的y坐标
 * @param[in] width:宽度
 * @param[in] height:高度
 * @return 无
 **/
void widget_set_location(widget_t *widget, s16 x, s16 y, s16 width, s16 height);

/**
 * @brief 获取Widget坐标及大小
 * @param[in] widget:控件指针
 * @return 控件的位置和大小
 **/
rect_t widget_get_location(const widget_t *widget);

/**
 * @brief 获取Widget绝对坐标及大小
 * @param[in] widget:需要设置的控件
 * @return 控件的绝对位置和大小
 **/
rect_t widget_get_absolute(const widget_t *widget);

/**
 * @brief 设置Widget的显示状态
 * @param[in] widget:需要设置的控件
 * @param[in] visible:true:可见  false:隐藏
 * @return 无
 **/
void widget_set_visible(widget_t *widget, bool visible);

/**
 * @brief 获取widget显示状态
 * @param[in] widget:控件指针
 * @return 显示状态 true:可见  false:隐藏
 **/
bool widget_get_visble(widget_t *widget);


//============= Widget Page ==============//


/**
 * @brief 创建一个Page型widget
 * @param[in] parent:父控件
 * @return page widget
 **/
widget_page_t *widget_page_create(widget_page_t *parent);

/**
 * @brief 判断widget是否为一个Page型
 * @param[in] widget:需要判断的控件
 * @return true or false
 **/
bool widget_is_page(const void *widget);

/**
 * @brief 设置Page client相对Page Window左上的坐标。0,0则对齐到左上角
 * @param[in] widget:需要设置的控件
 * @param[in] x:相对于左上角x坐标
 * @param[in] y:相对于左上角y坐标
 * @return 无
 **/
void widget_page_set_client(widget_page_t *widget, s16 x, s16 y);

/**
 * @brief 更新Page信息
 * @param[in] 无
 * @return 无
 **/
void widget_page_update(void);


//============= Widget Image ==============//

enum {
    ROT_MODE_NORMAL,        //普通旋转模式
    ROT_MODE_X,             //只旋转X (平行四边形, 2.5D效果)
};

/**
 * @brief 创建图片widget,支持缩放、裁剪和旋转
 * @param[in] parent:父控件
 * @param[in] res_addr:图片地址，在ui.h中获取
 * @return widget_image_t
 **/
widget_image_t *widget_image_create(widget_page_t *parent, u32 res_addr);

/**
 * @brief 设置图片
 * @param[in] img:图片控件
 * @param[in] res_addr:图片地址，在ui.h中获取
 * @return 无
 **/
void widget_image_set(widget_image_t *img, u32 res_addr);

/**
 * @brief 裁剪图片区域
 * @param[in] img:图片控件
 * @param[in] x:相对于图片左上角x坐标
 * @param[in] y:相对于图片左上角y坐标
 * @param[in] wid:截取宽度
 * @param[in] hei:截取高度
 * @return 无
 **/
void widget_image_cut(widget_image_t *img, s16 x, s16 y, s16 wid, s16 hei);

//============= Widget Text ==============//

/**
 * @brief 创建一个文本控件.
 * @param[in] parent:父控件
 * @param[in] max_word_cnt:最多多少个字(不分全半角)
 * @return 文本控件
 **/
widget_text_t *widget_text_create(widget_page_t *parent, u16 max_word_cnt);

/**
 * @brief 设置文本字库, 同时清空文本内容(需要重新设置文本内容)
 * @param[in] txt:文本控件
 * @param[in] font_res_addr:字库地址, 0则表示用回系统字体
 * @return 无
 **/
void widget_text_set_font(widget_text_t *txt, u32 font_res_addr);

/**
 * @brief 清空文本内容.
 * @param[in] txt:文本控件
 * @return 无
 **/
void widget_text_clear(widget_text_t *txt);

/**
 * @brief 设置文本内容.
 * @param[in] txt:文本控件
 * @param[in] text:字符串文本内容
 * @return 无
 **/
void widget_text_set(widget_text_t *txt, const char *text);

/**
 * @brief 设置文本颜色.
 * @param[in] txt:文本控件
 * @param[in] color:rgb565颜色值
 * @return 无
 **/
void widget_text_set_color(widget_text_t *txt, u16 color);

/**
 * @brief 设置是否支持自适应大小
 * @param[in] txt:文本控件
 * @param[in] autosize:true or false
 * @return 无
 **/
void widget_text_set_autosize(widget_text_t *txt, bool autosize);

/**
 * @brief 设置文本是否自动换行。
 * @param[in] txt:文本控件
 * @param[in] wordwrap:true or false
 * @return 无
 **/
void widget_text_set_wordwrap(widget_text_t *txt, bool wordwrap);

/**
 * @brief 设置文本排版方向
 * @param[in] txt:文本控件
 * @param[in] direction:true 倒序(阿拉伯/波斯/希伯来文从右到左)排版, false 正序(CJK等从左到右)排版
 * @return 无
 **/
void widget_text_set_direction(widget_text_t *txt, u8 direction);

/**
 * @brief 获取文本排版方向
 * @param[in] txt:文本控件
 * @param[in] direction:true 倒序(阿拉伯/波斯/希伯来文从右到左)排版, false 正序(CJK等从左到右)排版
 * @return 无
 **/
bool widget_text_get_direction(widget_text_t *txt);

/**
 * @brief 设置Client相对左上的坐标。0,0则对齐到左上角(用于文字滚动)
 * @param[in] txt:文本控件
 * @param[in] x:内容相对于控件左上角x坐标
 * @param[in] y:内容相对于控件左上角y坐标
 * @return 无
 **/
void widget_text_set_client(widget_text_t *txt, s16 x, s16 y);

/**
 * @brief 获取字体区域
 * @param[in] txt:文本控件
 * @return 文本内容的宽度和高度
 **/
area_t widget_text_get_area(widget_text_t *txt);

/**
 * @brief 以文本布局方向为参考系，获取文本框区域
 * @param[in] txt:文本控件
 * @return 文本框的宽度和高度
 **/
area_t widget_text_get_box_area_rel(widget_text_t *txt);

/**
 * @brief 获取文字行数
 * @param[in] txt:文本控件
 * @return ：行数
 **/
u8 widget_text_get_line_cnt(widget_text_t *txt);

/**
 * @brief 设置文本框文字是否等宽行间距
 * @param[in] textbox : 文本指针
 * @param[in] equal_hspace : 是否等宽行间距
 **/
void widget_text_set_equal_hspace(widget_text_t *txt, bool equal_hspace);

/**
 * @brief 获取文字行数
 * @param[in] txt:文本控件
 * @return ：像素点数
 **/
u16 widget_text_get_autoroll_circ_pixel(widget_text_t *txt);

/**
 * @brief 获取系统字高(典型值，非最大值)
 * @param[in] 无
 * @return 系统字库字体字高
 **/
u8 widget_text_get_height(void);

/**
 * @brief 设置排版方向
 * @param[in] right_align:排版方向是否右对齐
 * @return 无
 **/
void widget_text_set_right_align(widget_text_t *txt, bool right_align);

/**
 * @brief 设置滚动模式
 * @param[in] txt : 文本指针
 * @param[in] mode : 滚动模式，0:不滚动, 1:环形滚动 2、左右、普通滚动
 **/
void widget_text_set_autoroll_mode(widget_text_t *txt, u8 autoroll_mode);

/**
 * @brief 设置环形滚动间隔
 * @param[in] txt : 文本指针
 * @param[in] space_pixel : 环形滚动间隔，以英文空格为单位，默认是50
 **/
void widget_text_set_autoroll_circ_space(widget_text_t *txt, u16 space_pixel);

/**
 * @brief 获取排版方向
 * @param[in] 无
 * @return 排版方向1:右对齐 0:左对齐
 **/
bool widget_text_get_right_align(widget_text_t *txt);

/**
 * @brief 获取布局方向
 * @param[in] 无
 * @return 布局方向0:横向布局 1:竖向布局
 **/
int widget_text_get_layout(widget_text_t *txt);

//============= Widget Rect ==============//

/**
 * @brief 创建一个矩形控件
 * @param[in] parent：父控件
 * @return 矩形控件
 **/
widget_rect_t *widget_rect_create(widget_page_t *parent);

/**
 * @brief 设置矩形的填充颜色
 * @param[in] rect:矩形控件
 * @param[in] color:填充颜色
 * @return 无
 **/
void widget_rect_set_color(widget_rect_t *rect, u16 color);

#endif // _API_GUI_H
