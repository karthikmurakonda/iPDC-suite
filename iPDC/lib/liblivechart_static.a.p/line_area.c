/* line_area.c generated by valac 0.56.0, the Vala compiler
 * generated from line_area.vala, do not modify */

#include "livechart_static.h"
#include <glib.h>
#include <float.h>
#include <math.h>
#include <cairo-gobject.h>
#include <gdk/gdk.h>

enum  {
	LIVE_CHART_LINE_AREA_0_PROPERTY,
	LIVE_CHART_LINE_AREA_AREA_ALPHA_PROPERTY,
	LIVE_CHART_LINE_AREA_NUM_PROPERTIES
};
static GParamSpec* live_chart_line_area_properties[LIVE_CHART_LINE_AREA_NUM_PROPERTIES];
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

struct _LiveChartLineAreaPrivate {
	gdouble _area_alpha;
};

static gint LiveChartLineArea_private_offset;
static gpointer live_chart_line_area_parent_class = NULL;

static void live_chart_line_area_real_draw (LiveChartSerieRenderer* base,
                                     cairo_t* ctx,
                                     LiveChartConfig* config);
static void live_chart_line_area_finalize (GObject * obj);
static GType live_chart_line_area_get_type_once (void);
static void _vala_live_chart_line_area_get_property (GObject * object,
                                              guint property_id,
                                              GValue * value,
                                              GParamSpec * pspec);
static void _vala_live_chart_line_area_set_property (GObject * object,
                                              guint property_id,
                                              const GValue * value,
                                              GParamSpec * pspec);

static inline gpointer
live_chart_line_area_get_instance_private (LiveChartLineArea* self)
{
	return G_STRUCT_MEMBER_P (self, LiveChartLineArea_private_offset);
}

LiveChartLineArea*
live_chart_line_area_construct (GType object_type,
                                LiveChartValues* values)
{
	LiveChartLineArea * self = NULL;
#line 8 "../src/line_area.vala"
	g_return_val_if_fail (values != NULL, NULL);
#line 9 "../src/line_area.vala"
	self = (LiveChartLineArea*) live_chart_line_construct (object_type, values);
#line 8 "../src/line_area.vala"
	return self;
#line 58 "line_area.c"
}

LiveChartLineArea*
live_chart_line_area_new (LiveChartValues* values)
{
#line 8 "../src/line_area.vala"
	return live_chart_line_area_construct (LIVE_CHART_TYPE_LINE_AREA, values);
#line 66 "line_area.c"
}

static void
live_chart_line_area_real_draw (LiveChartSerieRenderer* base,
                                cairo_t* ctx,
                                LiveChartConfig* config)
{
	LiveChartLineArea * self;
	gboolean _tmp0_;
	gboolean _tmp1_;
#line 12 "../src/line_area.vala"
	self = (LiveChartLineArea*) base;
#line 12 "../src/line_area.vala"
	g_return_if_fail (ctx != NULL);
#line 12 "../src/line_area.vala"
	g_return_if_fail (config != NULL);
#line 13 "../src/line_area.vala"
	_tmp0_ = live_chart_drawable_get_visible ((LiveChartDrawable*) self);
#line 13 "../src/line_area.vala"
	_tmp1_ = _tmp0_;
#line 13 "../src/line_area.vala"
	if (_tmp1_) {
#line 89 "line_area.c"
		LiveChartPoints* points = NULL;
		LiveChartValues* _tmp2_;
		LiveChartPoints* _tmp3_;
		LiveChartPoints* _tmp4_;
		gint _tmp5_;
		gint _tmp6_;
#line 14 "../src/line_area.vala"
		_tmp2_ = ((LiveChartSerieRenderer*) self)->values;
#line 14 "../src/line_area.vala"
		_tmp3_ = live_chart_points_create (_tmp2_, config);
#line 14 "../src/line_area.vala"
		points = _tmp3_;
#line 15 "../src/line_area.vala"
		_tmp4_ = points;
#line 15 "../src/line_area.vala"
		_tmp5_ = live_chart_points_get_size (_tmp4_);
#line 15 "../src/line_area.vala"
		_tmp6_ = _tmp5_;
#line 15 "../src/line_area.vala"
		if (_tmp6_ > 0) {
#line 110 "line_area.c"
			LiveChartPoints* _tmp7_;
			LiveChartPoints* _tmp8_;
			LiveChartPoints* _tmp9_;
			LiveChartArea* area = NULL;
			LiveChartPoints* _tmp10_;
			GdkRGBA _tmp11_ = {0};
			GdkRGBA _tmp12_;
			gdouble _tmp13_;
			LiveChartArea* _tmp14_;
			LiveChartArea* _tmp15_;
#line 16 "../src/line_area.vala"
			_tmp7_ = points;
#line 16 "../src/line_area.vala"
			_tmp8_ = live_chart_line_draw_line ((LiveChartLine*) self, _tmp7_, ctx, config);
#line 16 "../src/line_area.vala"
			_tmp9_ = _tmp8_;
#line 16 "../src/line_area.vala"
			_g_object_unref0 (_tmp9_);
#line 17 "../src/line_area.vala"
			cairo_stroke_preserve (ctx);
#line 19 "../src/line_area.vala"
			_tmp10_ = points;
#line 19 "../src/line_area.vala"
			live_chart_serie_renderer_get_main_color ((LiveChartSerieRenderer*) self, &_tmp11_);
#line 19 "../src/line_area.vala"
			_tmp12_ = _tmp11_;
#line 19 "../src/line_area.vala"
			_tmp13_ = self->priv->_area_alpha;
#line 19 "../src/line_area.vala"
			_tmp14_ = live_chart_area_new (_tmp10_, &_tmp12_, _tmp13_);
#line 19 "../src/line_area.vala"
			area = _tmp14_;
#line 20 "../src/line_area.vala"
			_tmp15_ = area;
#line 20 "../src/line_area.vala"
			live_chart_drawable_draw ((LiveChartDrawable*) _tmp15_, ctx, config);
#line 21 "../src/line_area.vala"
			cairo_fill (ctx);
#line 15 "../src/line_area.vala"
			_g_object_unref0 (area);
#line 151 "line_area.c"
		}
#line 24 "../src/line_area.vala"
		cairo_stroke (ctx);
#line 13 "../src/line_area.vala"
		_g_object_unref0 (points);
#line 157 "line_area.c"
	}
}

gdouble
live_chart_line_area_get_area_alpha (LiveChartLineArea* self)
{
	gdouble result;
#line 6 "../src/line_area.vala"
	g_return_val_if_fail (self != NULL, 0.0);
#line 6 "../src/line_area.vala"
	result = self->priv->_area_alpha;
#line 6 "../src/line_area.vala"
	return result;
#line 171 "line_area.c"
}

void
live_chart_line_area_set_area_alpha (LiveChartLineArea* self,
                                     gdouble value)
{
	gdouble old_value;
#line 6 "../src/line_area.vala"
	g_return_if_fail (self != NULL);
#line 6 "../src/line_area.vala"
	old_value = live_chart_line_area_get_area_alpha (self);
#line 6 "../src/line_area.vala"
	if (old_value != value) {
#line 6 "../src/line_area.vala"
		self->priv->_area_alpha = value;
#line 6 "../src/line_area.vala"
		g_object_notify_by_pspec ((GObject *) self, live_chart_line_area_properties[LIVE_CHART_LINE_AREA_AREA_ALPHA_PROPERTY]);
#line 189 "line_area.c"
	}
}

static void
live_chart_line_area_class_init (LiveChartLineAreaClass * klass,
                                 gpointer klass_data)
{
#line 4 "../src/line_area.vala"
	live_chart_line_area_parent_class = g_type_class_peek_parent (klass);
#line 4 "../src/line_area.vala"
	g_type_class_adjust_private_offset (klass, &LiveChartLineArea_private_offset);
#line 4 "../src/line_area.vala"
	((LiveChartSerieRendererClass *) klass)->draw = (void (*) (LiveChartSerieRenderer*, cairo_t*, LiveChartConfig*)) live_chart_line_area_real_draw;
#line 4 "../src/line_area.vala"
	G_OBJECT_CLASS (klass)->get_property = _vala_live_chart_line_area_get_property;
#line 4 "../src/line_area.vala"
	G_OBJECT_CLASS (klass)->set_property = _vala_live_chart_line_area_set_property;
#line 4 "../src/line_area.vala"
	G_OBJECT_CLASS (klass)->finalize = live_chart_line_area_finalize;
#line 4 "../src/line_area.vala"
	g_object_class_install_property (G_OBJECT_CLASS (klass), LIVE_CHART_LINE_AREA_AREA_ALPHA_PROPERTY, live_chart_line_area_properties[LIVE_CHART_LINE_AREA_AREA_ALPHA_PROPERTY] = g_param_spec_double ("area-alpha", "area-alpha", "area-alpha", -G_MAXDOUBLE, G_MAXDOUBLE, 0.1, G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE));
#line 211 "line_area.c"
}

static void
live_chart_line_area_instance_init (LiveChartLineArea * self,
                                    gpointer klass)
{
#line 4 "../src/line_area.vala"
	self->priv = live_chart_line_area_get_instance_private (self);
#line 6 "../src/line_area.vala"
	self->priv->_area_alpha = 0.1;
#line 222 "line_area.c"
}

static void
live_chart_line_area_finalize (GObject * obj)
{
	LiveChartLineArea * self;
#line 4 "../src/line_area.vala"
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, LIVE_CHART_TYPE_LINE_AREA, LiveChartLineArea);
#line 4 "../src/line_area.vala"
	G_OBJECT_CLASS (live_chart_line_area_parent_class)->finalize (obj);
#line 233 "line_area.c"
}

static GType
live_chart_line_area_get_type_once (void)
{
	static const GTypeInfo g_define_type_info = { sizeof (LiveChartLineAreaClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) live_chart_line_area_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (LiveChartLineArea), 0, (GInstanceInitFunc) live_chart_line_area_instance_init, NULL };
	GType live_chart_line_area_type_id;
	live_chart_line_area_type_id = g_type_register_static (LIVE_CHART_TYPE_LINE, "LiveChartLineArea", &g_define_type_info, 0);
	LiveChartLineArea_private_offset = g_type_add_instance_private (live_chart_line_area_type_id, sizeof (LiveChartLineAreaPrivate));
	return live_chart_line_area_type_id;
}

GType
live_chart_line_area_get_type (void)
{
	static volatile gsize live_chart_line_area_type_id__once = 0;
	if (g_once_init_enter (&live_chart_line_area_type_id__once)) {
		GType live_chart_line_area_type_id;
		live_chart_line_area_type_id = live_chart_line_area_get_type_once ();
		g_once_init_leave (&live_chart_line_area_type_id__once, live_chart_line_area_type_id);
	}
	return live_chart_line_area_type_id__once;
}

static void
_vala_live_chart_line_area_get_property (GObject * object,
                                         guint property_id,
                                         GValue * value,
                                         GParamSpec * pspec)
{
	LiveChartLineArea * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, LIVE_CHART_TYPE_LINE_AREA, LiveChartLineArea);
#line 4 "../src/line_area.vala"
	switch (property_id) {
#line 4 "../src/line_area.vala"
		case LIVE_CHART_LINE_AREA_AREA_ALPHA_PROPERTY:
#line 4 "../src/line_area.vala"
		g_value_set_double (value, live_chart_line_area_get_area_alpha (self));
#line 4 "../src/line_area.vala"
		break;
#line 274 "line_area.c"
		default:
#line 4 "../src/line_area.vala"
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
#line 4 "../src/line_area.vala"
		break;
#line 280 "line_area.c"
	}
}

static void
_vala_live_chart_line_area_set_property (GObject * object,
                                         guint property_id,
                                         const GValue * value,
                                         GParamSpec * pspec)
{
	LiveChartLineArea * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, LIVE_CHART_TYPE_LINE_AREA, LiveChartLineArea);
#line 4 "../src/line_area.vala"
	switch (property_id) {
#line 4 "../src/line_area.vala"
		case LIVE_CHART_LINE_AREA_AREA_ALPHA_PROPERTY:
#line 4 "../src/line_area.vala"
		live_chart_line_area_set_area_alpha (self, g_value_get_double (value));
#line 4 "../src/line_area.vala"
		break;
#line 300 "line_area.c"
		default:
#line 4 "../src/line_area.vala"
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
#line 4 "../src/line_area.vala"
		break;
#line 306 "line_area.c"
	}
}
