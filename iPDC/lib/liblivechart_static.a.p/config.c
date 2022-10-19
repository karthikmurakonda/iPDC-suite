/* config.c generated by valac 0.56.0, the Vala compiler
 * generated from config.vala, do not modify */

#include "livechart_static.h"
#include <string.h>
#include <glib.h>
#include <cairo-gobject.h>
#include <stdlib.h>
#include <gobject/gvaluecollector.h>

#define _live_chart_yaxis_unref0(var) ((var == NULL) ? NULL : (var = (live_chart_yaxis_unref (var), NULL)))
#define _live_chart_xaxis_unref0(var) ((var == NULL) ? NULL : (var = (live_chart_xaxis_unref (var), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
typedef struct _LiveChartParamSpecConfig LiveChartParamSpecConfig;

struct _LiveChartConfigPrivate {
	gint _width;
	gint _height;
};

struct _LiveChartParamSpecConfig {
	GParamSpec parent_instance;
};

static gint LiveChartConfig_private_offset;
static gpointer live_chart_config_parent_class = NULL;

static void live_chart_config_configure_y_max_labels_extents (LiveChartConfig* self,
                                                       cairo_t* ctx);
static void live_chart_config_configure_x_max_labels_extents (LiveChartConfig* self,
                                                       cairo_t* ctx);
static void live_chart_config_finalize (LiveChartConfig * obj);
static GType live_chart_config_get_type_once (void);

static GType
live_chart_auto_padding_get_type_once (void)
{
	static const GFlagsValue values[] = {{LIVE_CHART_AUTO_PADDING_NONE, "LIVE_CHART_AUTO_PADDING_NONE", "none"}, {LIVE_CHART_AUTO_PADDING_TOP, "LIVE_CHART_AUTO_PADDING_TOP", "top"}, {LIVE_CHART_AUTO_PADDING_RIGHT, "LIVE_CHART_AUTO_PADDING_RIGHT", "right"}, {LIVE_CHART_AUTO_PADDING_BOTTOM, "LIVE_CHART_AUTO_PADDING_BOTTOM", "bottom"}, {LIVE_CHART_AUTO_PADDING_LEFT, "LIVE_CHART_AUTO_PADDING_LEFT", "left"}, {0, NULL, NULL}};
	GType live_chart_auto_padding_type_id;
	live_chart_auto_padding_type_id = g_flags_register_static ("LiveChartAutoPadding", values);
	return live_chart_auto_padding_type_id;
}

GType
live_chart_auto_padding_get_type (void)
{
	static volatile gsize live_chart_auto_padding_type_id__once = 0;
	if (g_once_init_enter (&live_chart_auto_padding_type_id__once)) {
		GType live_chart_auto_padding_type_id;
		live_chart_auto_padding_type_id = live_chart_auto_padding_get_type_once ();
		g_once_init_leave (&live_chart_auto_padding_type_id__once, live_chart_auto_padding_type_id);
	}
	return live_chart_auto_padding_type_id__once;
}

void
live_chart_padding_init (LiveChartPadding *self)
{
#line 21 "../src/config.vala"
	memset (self, 0, sizeof (LiveChartPadding));
#line 22 "../src/config.vala"
	(*self).smart = ((LIVE_CHART_AUTO_PADDING_TOP | LIVE_CHART_AUTO_PADDING_RIGHT) | LIVE_CHART_AUTO_PADDING_BOTTOM) | LIVE_CHART_AUTO_PADDING_LEFT;
#line 23 "../src/config.vala"
	(*self).top = 20;
#line 24 "../src/config.vala"
	(*self).right = 20;
#line 25 "../src/config.vala"
	(*self).bottom = 20;
#line 26 "../src/config.vala"
	(*self).left = 20;
#line 73 "config.c"
}

LiveChartPadding*
live_chart_padding_dup (const LiveChartPadding* self)
{
	LiveChartPadding* dup;
#line 14 "../src/config.vala"
	dup = g_new0 (LiveChartPadding, 1);
#line 14 "../src/config.vala"
	memcpy (dup, self, sizeof (LiveChartPadding));
#line 14 "../src/config.vala"
	return dup;
#line 86 "config.c"
}

void
live_chart_padding_free (LiveChartPadding* self)
{
#line 14 "../src/config.vala"
	g_free (self);
#line 94 "config.c"
}

static GType
live_chart_padding_get_type_once (void)
{
	GType live_chart_padding_type_id;
	live_chart_padding_type_id = g_boxed_type_register_static ("LiveChartPadding", (GBoxedCopyFunc) live_chart_padding_dup, (GBoxedFreeFunc) live_chart_padding_free);
	return live_chart_padding_type_id;
}

GType
live_chart_padding_get_type (void)
{
	static volatile gsize live_chart_padding_type_id__once = 0;
	if (g_once_init_enter (&live_chart_padding_type_id__once)) {
		GType live_chart_padding_type_id;
		live_chart_padding_type_id = live_chart_padding_get_type_once ();
		g_once_init_leave (&live_chart_padding_type_id__once, live_chart_padding_type_id);
	}
	return live_chart_padding_type_id__once;
}

LiveChartBoundary*
live_chart_boundary_dup (const LiveChartBoundary* self)
{
	LiveChartBoundary* dup;
#line 30 "../src/config.vala"
	dup = g_new0 (LiveChartBoundary, 1);
#line 30 "../src/config.vala"
	memcpy (dup, self, sizeof (LiveChartBoundary));
#line 30 "../src/config.vala"
	return dup;
#line 127 "config.c"
}

void
live_chart_boundary_free (LiveChartBoundary* self)
{
#line 30 "../src/config.vala"
	g_free (self);
#line 135 "config.c"
}

static GType
live_chart_boundary_get_type_once (void)
{
	GType live_chart_boundary_type_id;
	live_chart_boundary_type_id = g_boxed_type_register_static ("LiveChartBoundary", (GBoxedCopyFunc) live_chart_boundary_dup, (GBoxedFreeFunc) live_chart_boundary_free);
	return live_chart_boundary_type_id;
}

GType
live_chart_boundary_get_type (void)
{
	static volatile gsize live_chart_boundary_type_id__once = 0;
	if (g_once_init_enter (&live_chart_boundary_type_id__once)) {
		GType live_chart_boundary_type_id;
		live_chart_boundary_type_id = live_chart_boundary_get_type_once ();
		g_once_init_leave (&live_chart_boundary_type_id__once, live_chart_boundary_type_id);
	}
	return live_chart_boundary_type_id__once;
}

LiveChartBoundaries*
live_chart_boundaries_dup (const LiveChartBoundaries* self)
{
	LiveChartBoundaries* dup;
#line 35 "../src/config.vala"
	dup = g_new0 (LiveChartBoundaries, 1);
#line 35 "../src/config.vala"
	memcpy (dup, self, sizeof (LiveChartBoundaries));
#line 35 "../src/config.vala"
	return dup;
#line 168 "config.c"
}

void
live_chart_boundaries_free (LiveChartBoundaries* self)
{
#line 35 "../src/config.vala"
	g_free (self);
#line 176 "config.c"
}

static GType
live_chart_boundaries_get_type_once (void)
{
	GType live_chart_boundaries_type_id;
	live_chart_boundaries_type_id = g_boxed_type_register_static ("LiveChartBoundaries", (GBoxedCopyFunc) live_chart_boundaries_dup, (GBoxedFreeFunc) live_chart_boundaries_free);
	return live_chart_boundaries_type_id;
}

GType
live_chart_boundaries_get_type (void)
{
	static volatile gsize live_chart_boundaries_type_id__once = 0;
	if (g_once_init_enter (&live_chart_boundaries_type_id__once)) {
		GType live_chart_boundaries_type_id;
		live_chart_boundaries_type_id = live_chart_boundaries_get_type_once ();
		g_once_init_leave (&live_chart_boundaries_type_id__once, live_chart_boundaries_type_id);
	}
	return live_chart_boundaries_type_id__once;
}

static inline gpointer
live_chart_config_get_instance_private (LiveChartConfig* self)
{
	return G_STRUCT_MEMBER_P (self, LiveChartConfig_private_offset);
}

void
live_chart_config_boundaries (LiveChartConfig* self,
                              LiveChartBoundaries* result)
{
	LiveChartPadding _tmp0_;
	gint _tmp1_;
	LiveChartPadding _tmp2_;
	LiveChartBoundary _tmp3_ = {0};
	LiveChartPadding _tmp4_;
	gint _tmp5_;
	LiveChartPadding _tmp6_;
	LiveChartBoundary _tmp7_ = {0};
	gint _tmp8_;
	LiveChartPadding _tmp9_;
	LiveChartPadding _tmp10_;
	gint _tmp11_;
	LiveChartPadding _tmp12_;
	LiveChartPadding _tmp13_;
	LiveChartBoundaries _tmp14_ = {0};
#line 59 "../src/config.vala"
	g_return_if_fail (self != NULL);
#line 60 "../src/config.vala"
	_tmp0_ = self->padding;
#line 60 "../src/config.vala"
	_tmp1_ = self->priv->_width;
#line 60 "../src/config.vala"
	_tmp2_ = self->padding;
#line 60 "../src/config.vala"
	_tmp3_.min = _tmp0_.left;
#line 60 "../src/config.vala"
	_tmp3_.max = _tmp1_ - _tmp2_.right;
#line 60 "../src/config.vala"
	_tmp4_ = self->padding;
#line 60 "../src/config.vala"
	_tmp5_ = self->priv->_height;
#line 60 "../src/config.vala"
	_tmp6_ = self->padding;
#line 60 "../src/config.vala"
	_tmp7_.min = _tmp4_.top;
#line 60 "../src/config.vala"
	_tmp7_.max = _tmp5_ - _tmp6_.bottom;
#line 60 "../src/config.vala"
	_tmp8_ = self->priv->_width;
#line 60 "../src/config.vala"
	_tmp9_ = self->padding;
#line 60 "../src/config.vala"
	_tmp10_ = self->padding;
#line 60 "../src/config.vala"
	_tmp11_ = self->priv->_height;
#line 60 "../src/config.vala"
	_tmp12_ = self->padding;
#line 60 "../src/config.vala"
	_tmp13_ = self->padding;
#line 60 "../src/config.vala"
	memset (&_tmp14_, 0, sizeof (LiveChartBoundaries));
#line 60 "../src/config.vala"
	_tmp14_.x = _tmp3_;
#line 60 "../src/config.vala"
	_tmp14_.y = _tmp7_;
#line 60 "../src/config.vala"
	_tmp14_.width = (_tmp8_ - _tmp9_.right) - _tmp10_.left;
#line 60 "../src/config.vala"
	_tmp14_.height = (_tmp11_ - _tmp12_.bottom) - _tmp13_.top;
#line 60 "../src/config.vala"
	*result = _tmp14_;
#line 60 "../src/config.vala"
	return;
#line 272 "config.c"
}

void
live_chart_config_configure (LiveChartConfig* self,
                             cairo_t* ctx,
                             LiveChartLegend* legend)
{
	LiveChartPadding _tmp0_;
	LiveChartPadding _tmp5_;
	LiveChartPadding _tmp10_;
	LiveChartPadding _tmp15_;
	gboolean _tmp16_ = FALSE;
	LiveChartYAxis* _tmp20_;
	LiveChartBoundaries _tmp21_ = {0};
#line 68 "../src/config.vala"
	g_return_if_fail (self != NULL);
#line 68 "../src/config.vala"
	g_return_if_fail (ctx != NULL);
#line 69 "../src/config.vala"
	live_chart_config_configure_y_max_labels_extents (self, ctx);
#line 70 "../src/config.vala"
	live_chart_config_configure_x_max_labels_extents (self, ctx);
#line 72 "../src/config.vala"
	_tmp0_ = self->padding;
#line 72 "../src/config.vala"
	if ((_tmp0_.smart & LIVE_CHART_AUTO_PADDING_RIGHT) == LIVE_CHART_AUTO_PADDING_RIGHT) {
#line 299 "config.c"
		LiveChartXAxis* _tmp1_;
		LiveChartLabels* _tmp2_;
		cairo_text_extents_t _tmp3_ = {0};
		cairo_text_extents_t _tmp4_;
#line 72 "../src/config.vala"
		_tmp1_ = self->x_axis;
#line 72 "../src/config.vala"
		_tmp2_ = _tmp1_->labels;
#line 72 "../src/config.vala"
		live_chart_labels_get_extents (_tmp2_, &_tmp3_);
#line 72 "../src/config.vala"
		_tmp4_ = _tmp3_;
#line 72 "../src/config.vala"
		self->padding.right = 10 + (((gint) _tmp4_.width) / 2);
#line 314 "config.c"
	}
#line 73 "../src/config.vala"
	_tmp5_ = self->padding;
#line 73 "../src/config.vala"
	if ((_tmp5_.smart & LIVE_CHART_AUTO_PADDING_LEFT) == LIVE_CHART_AUTO_PADDING_LEFT) {
#line 320 "config.c"
		LiveChartYAxis* _tmp6_;
		LiveChartLabels* _tmp7_;
		cairo_text_extents_t _tmp8_ = {0};
		cairo_text_extents_t _tmp9_;
#line 73 "../src/config.vala"
		_tmp6_ = self->y_axis;
#line 73 "../src/config.vala"
		_tmp7_ = _tmp6_->labels;
#line 73 "../src/config.vala"
		live_chart_labels_get_extents (_tmp7_, &_tmp8_);
#line 73 "../src/config.vala"
		_tmp9_ = _tmp8_;
#line 73 "../src/config.vala"
		self->padding.left = (gint) _tmp9_.width;
#line 335 "config.c"
	}
#line 74 "../src/config.vala"
	_tmp10_ = self->padding;
#line 74 "../src/config.vala"
	if ((_tmp10_.smart & LIVE_CHART_AUTO_PADDING_BOTTOM) == LIVE_CHART_AUTO_PADDING_BOTTOM) {
#line 341 "config.c"
		LiveChartXAxis* _tmp11_;
		LiveChartLabels* _tmp12_;
		cairo_text_extents_t _tmp13_ = {0};
		cairo_text_extents_t _tmp14_;
#line 74 "../src/config.vala"
		_tmp11_ = self->x_axis;
#line 74 "../src/config.vala"
		_tmp12_ = _tmp11_->labels;
#line 74 "../src/config.vala"
		live_chart_labels_get_extents (_tmp12_, &_tmp13_);
#line 74 "../src/config.vala"
		_tmp14_ = _tmp13_;
#line 74 "../src/config.vala"
		self->padding.bottom = 15 + ((gint) _tmp14_.height);
#line 356 "config.c"
	}
#line 75 "../src/config.vala"
	_tmp15_ = self->padding;
#line 75 "../src/config.vala"
	if ((_tmp15_.smart & LIVE_CHART_AUTO_PADDING_TOP) == LIVE_CHART_AUTO_PADDING_TOP) {
#line 75 "../src/config.vala"
		self->padding.top = 10;
#line 364 "config.c"
	}
#line 77 "../src/config.vala"
	if (legend != NULL) {
#line 368 "config.c"
		LiveChartPadding _tmp17_;
#line 77 "../src/config.vala"
		_tmp17_ = self->padding;
#line 77 "../src/config.vala"
		_tmp16_ = (_tmp17_.smart & LIVE_CHART_AUTO_PADDING_BOTTOM) == LIVE_CHART_AUTO_PADDING_BOTTOM;
#line 374 "config.c"
	} else {
#line 77 "../src/config.vala"
		_tmp16_ = FALSE;
#line 378 "config.c"
	}
#line 77 "../src/config.vala"
	if (_tmp16_) {
#line 382 "config.c"
		LiveChartPadding _tmp18_;
		LiveChartBoundingBox _tmp19_ = {0};
#line 77 "../src/config.vala"
		_tmp18_ = self->padding;
#line 77 "../src/config.vala"
		live_chart_drawable_get_bounding_box ((LiveChartDrawable*) legend, &_tmp19_);
#line 77 "../src/config.vala"
		self->padding.bottom = (_tmp18_.bottom + ((gint) _tmp19_.height)) + 5;
#line 391 "config.c"
	}
#line 79 "../src/config.vala"
	_tmp20_ = self->y_axis;
#line 79 "../src/config.vala"
	live_chart_config_boundaries (self, &_tmp21_);
#line 79 "../src/config.vala"
	live_chart_yaxis_update (_tmp20_, _tmp21_.height);
#line 399 "config.c"
}

static void
live_chart_config_configure_y_max_labels_extents (LiveChartConfig* self,
                                                  cairo_t* ctx)
{
	cairo_text_extents_t extents = {0};
	gboolean _tmp0_ = FALSE;
	LiveChartYAxis* _tmp1_;
	gboolean _tmp2_;
	gboolean _tmp3_;
	LiveChartYAxis* _tmp29_;
	LiveChartLabels* _tmp30_;
	cairo_text_extents_t _tmp31_;
#line 82 "../src/config.vala"
	g_return_if_fail (self != NULL);
#line 82 "../src/config.vala"
	g_return_if_fail (ctx != NULL);
#line 84 "../src/config.vala"
	_tmp1_ = self->y_axis;
#line 84 "../src/config.vala"
	_tmp2_ = live_chart_yaxis_get_visible (_tmp1_);
#line 84 "../src/config.vala"
	_tmp3_ = _tmp2_;
#line 84 "../src/config.vala"
	if (_tmp3_) {
#line 426 "config.c"
		LiveChartYAxis* _tmp4_;
		LiveChartLabels* _tmp5_;
		gboolean _tmp6_;
		gboolean _tmp7_;
#line 84 "../src/config.vala"
		_tmp4_ = self->y_axis;
#line 84 "../src/config.vala"
		_tmp5_ = _tmp4_->labels;
#line 84 "../src/config.vala"
		_tmp6_ = live_chart_labels_get_visible (_tmp5_);
#line 84 "../src/config.vala"
		_tmp7_ = _tmp6_;
#line 84 "../src/config.vala"
		_tmp0_ = _tmp7_;
#line 441 "config.c"
	} else {
#line 84 "../src/config.vala"
		_tmp0_ = FALSE;
#line 445 "config.c"
	}
#line 84 "../src/config.vala"
	if (_tmp0_) {
#line 449 "config.c"
		LiveChartYAxis* _tmp8_;
		LiveChartLabels* _tmp9_;
		LiveChartFont* _tmp10_;
		LiveChartFont* _tmp11_;
		const gchar* _tmp12_ = NULL;
		LiveChartYAxis* _tmp13_;
		const gchar* _tmp14_;
		const gchar* _tmp15_;
		gint _tmp16_;
		gint _tmp17_;
		gchar* spaces = NULL;
		gchar* _tmp21_;
		LiveChartYAxis* _tmp22_;
		gchar* _tmp23_;
		gchar* _tmp24_;
		const gchar* _tmp25_;
		gchar* _tmp26_;
		gchar* _tmp27_;
		cairo_text_extents_t _tmp28_ = {0};
#line 85 "../src/config.vala"
		_tmp8_ = self->y_axis;
#line 85 "../src/config.vala"
		_tmp9_ = _tmp8_->labels;
#line 85 "../src/config.vala"
		_tmp10_ = live_chart_labels_get_font (_tmp9_);
#line 85 "../src/config.vala"
		_tmp11_ = _tmp10_;
#line 85 "../src/config.vala"
		live_chart_font_configure (_tmp11_, ctx);
#line 87 "../src/config.vala"
		_tmp13_ = self->y_axis;
#line 87 "../src/config.vala"
		_tmp14_ = live_chart_yaxis_get_unit (_tmp13_);
#line 87 "../src/config.vala"
		_tmp15_ = _tmp14_;
#line 87 "../src/config.vala"
		_tmp16_ = strlen (_tmp15_);
#line 87 "../src/config.vala"
		_tmp17_ = _tmp16_;
#line 87 "../src/config.vala"
		if (_tmp17_ == 0) {
#line 87 "../src/config.vala"
			_tmp12_ = "__";
#line 493 "config.c"
		} else {
			LiveChartYAxis* _tmp18_;
			const gchar* _tmp19_;
			const gchar* _tmp20_;
#line 87 "../src/config.vala"
			_tmp18_ = self->y_axis;
#line 87 "../src/config.vala"
			_tmp19_ = live_chart_yaxis_get_unit (_tmp18_);
#line 87 "../src/config.vala"
			_tmp20_ = _tmp19_;
#line 87 "../src/config.vala"
			_tmp12_ = _tmp20_;
#line 506 "config.c"
		}
#line 87 "../src/config.vala"
		_tmp21_ = g_strdup (_tmp12_);
#line 87 "../src/config.vala"
		spaces = _tmp21_;
#line 88 "../src/config.vala"
		_tmp22_ = self->y_axis;
#line 88 "../src/config.vala"
		_tmp23_ = live_chart_yaxis_get_max_displayed_value (_tmp22_);
#line 88 "../src/config.vala"
		_tmp24_ = _tmp23_;
#line 88 "../src/config.vala"
		_tmp25_ = spaces;
#line 88 "../src/config.vala"
		_tmp26_ = g_strconcat (_tmp24_, _tmp25_, NULL);
#line 88 "../src/config.vala"
		_tmp27_ = _tmp26_;
#line 88 "../src/config.vala"
		cairo_text_extents (ctx, _tmp27_, &_tmp28_);
#line 88 "../src/config.vala"
		extents = _tmp28_;
#line 88 "../src/config.vala"
		_g_free0 (_tmp27_);
#line 88 "../src/config.vala"
		_g_free0 (_tmp24_);
#line 84 "../src/config.vala"
		_g_free0 (spaces);
#line 534 "config.c"
	} else {
#line 90 "../src/config.vala"
		memset (&extents, 0, sizeof (cairo_text_extents_t));
#line 91 "../src/config.vala"
		extents.height = 0.0;
#line 92 "../src/config.vala"
		extents.width = 0.0;
#line 93 "../src/config.vala"
		extents.x_advance = 0.0;
#line 94 "../src/config.vala"
		extents.x_bearing = 0.0;
#line 95 "../src/config.vala"
		extents.y_advance = 0.0;
#line 96 "../src/config.vala"
		extents.y_bearing = 0.0;
#line 550 "config.c"
	}
#line 99 "../src/config.vala"
	_tmp29_ = self->y_axis;
#line 99 "../src/config.vala"
	_tmp30_ = _tmp29_->labels;
#line 99 "../src/config.vala"
	_tmp31_ = extents;
#line 99 "../src/config.vala"
	live_chart_labels_set_extents (_tmp30_, &_tmp31_);
#line 560 "config.c"
}

static void
live_chart_config_configure_x_max_labels_extents (LiveChartConfig* self,
                                                  cairo_t* ctx)
{
	cairo_text_extents_t extents = {0};
	gboolean _tmp0_ = FALSE;
	LiveChartXAxis* _tmp1_;
	gboolean _tmp2_;
	gboolean _tmp3_;
	LiveChartXAxis* _tmp15_;
	LiveChartLabels* _tmp16_;
	cairo_text_extents_t _tmp17_;
#line 102 "../src/config.vala"
	g_return_if_fail (self != NULL);
#line 102 "../src/config.vala"
	g_return_if_fail (ctx != NULL);
#line 104 "../src/config.vala"
	_tmp1_ = self->x_axis;
#line 104 "../src/config.vala"
	_tmp2_ = live_chart_xaxis_get_visible (_tmp1_);
#line 104 "../src/config.vala"
	_tmp3_ = _tmp2_;
#line 104 "../src/config.vala"
	if (_tmp3_) {
#line 587 "config.c"
		LiveChartXAxis* _tmp4_;
		LiveChartLabels* _tmp5_;
		gboolean _tmp6_;
		gboolean _tmp7_;
#line 104 "../src/config.vala"
		_tmp4_ = self->x_axis;
#line 104 "../src/config.vala"
		_tmp5_ = _tmp4_->labels;
#line 104 "../src/config.vala"
		_tmp6_ = live_chart_labels_get_visible (_tmp5_);
#line 104 "../src/config.vala"
		_tmp7_ = _tmp6_;
#line 104 "../src/config.vala"
		_tmp0_ = _tmp7_;
#line 602 "config.c"
	} else {
#line 104 "../src/config.vala"
		_tmp0_ = FALSE;
#line 606 "config.c"
	}
#line 104 "../src/config.vala"
	if (_tmp0_) {
#line 610 "config.c"
		LiveChartXAxis* _tmp8_;
		LiveChartLabels* _tmp9_;
		LiveChartFont* _tmp10_;
		LiveChartFont* _tmp11_;
		gchar* time_format = NULL;
		gchar* _tmp12_;
		const gchar* _tmp13_;
		cairo_text_extents_t _tmp14_ = {0};
#line 105 "../src/config.vala"
		_tmp8_ = self->x_axis;
#line 105 "../src/config.vala"
		_tmp9_ = _tmp8_->labels;
#line 105 "../src/config.vala"
		_tmp10_ = live_chart_labels_get_font (_tmp9_);
#line 105 "../src/config.vala"
		_tmp11_ = _tmp10_;
#line 105 "../src/config.vala"
		live_chart_font_configure (_tmp11_, ctx);
#line 106 "../src/config.vala"
		_tmp12_ = g_strdup ("00:00:00");
#line 106 "../src/config.vala"
		time_format = _tmp12_;
#line 107 "../src/config.vala"
		_tmp13_ = time_format;
#line 107 "../src/config.vala"
		cairo_text_extents (ctx, _tmp13_, &_tmp14_);
#line 107 "../src/config.vala"
		extents = _tmp14_;
#line 104 "../src/config.vala"
		_g_free0 (time_format);
#line 641 "config.c"
	} else {
#line 109 "../src/config.vala"
		memset (&extents, 0, sizeof (cairo_text_extents_t));
#line 110 "../src/config.vala"
		extents.height = 0.0;
#line 111 "../src/config.vala"
		extents.width = 0.0;
#line 112 "../src/config.vala"
		extents.x_advance = 0.0;
#line 113 "../src/config.vala"
		extents.x_bearing = 0.0;
#line 114 "../src/config.vala"
		extents.y_advance = 0.0;
#line 115 "../src/config.vala"
		extents.y_bearing = 0.0;
#line 657 "config.c"
	}
#line 118 "../src/config.vala"
	_tmp15_ = self->x_axis;
#line 118 "../src/config.vala"
	_tmp16_ = _tmp15_->labels;
#line 118 "../src/config.vala"
	_tmp17_ = extents;
#line 118 "../src/config.vala"
	live_chart_labels_set_extents (_tmp16_, &_tmp17_);
#line 667 "config.c"
}

LiveChartConfig*
live_chart_config_construct (GType object_type)
{
	LiveChartConfig* self = NULL;
#line 42 "../src/config.vala"
	self = (LiveChartConfig*) g_type_create_instance (object_type);
#line 42 "../src/config.vala"
	return self;
#line 678 "config.c"
}

LiveChartConfig*
live_chart_config_new (void)
{
#line 42 "../src/config.vala"
	return live_chart_config_construct (LIVE_CHART_TYPE_CONFIG);
#line 686 "config.c"
}

gint
live_chart_config_get_width (LiveChartConfig* self)
{
	gint result;
#line 45 "../src/config.vala"
	g_return_val_if_fail (self != NULL, 0);
#line 45 "../src/config.vala"
	result = self->priv->_width;
#line 45 "../src/config.vala"
	return result;
#line 699 "config.c"
}

void
live_chart_config_set_width (LiveChartConfig* self,
                             gint value)
{
#line 45 "../src/config.vala"
	g_return_if_fail (self != NULL);
#line 45 "../src/config.vala"
	self->priv->_width = value;
#line 710 "config.c"
}

gint
live_chart_config_get_height (LiveChartConfig* self)
{
	gint result;
#line 49 "../src/config.vala"
	g_return_val_if_fail (self != NULL, 0);
#line 49 "../src/config.vala"
	result = self->priv->_height;
#line 49 "../src/config.vala"
	return result;
#line 723 "config.c"
}

void
live_chart_config_set_height (LiveChartConfig* self,
                              gint value)
{
#line 49 "../src/config.vala"
	g_return_if_fail (self != NULL);
#line 49 "../src/config.vala"
	self->priv->_height = value;
#line 734 "config.c"
}

static void
live_chart_value_config_init (GValue* value)
{
#line 42 "../src/config.vala"
	value->data[0].v_pointer = NULL;
#line 742 "config.c"
}

static void
live_chart_value_config_free_value (GValue* value)
{
#line 42 "../src/config.vala"
	if (value->data[0].v_pointer) {
#line 42 "../src/config.vala"
		live_chart_config_unref (value->data[0].v_pointer);
#line 752 "config.c"
	}
}

static void
live_chart_value_config_copy_value (const GValue* src_value,
                                    GValue* dest_value)
{
#line 42 "../src/config.vala"
	if (src_value->data[0].v_pointer) {
#line 42 "../src/config.vala"
		dest_value->data[0].v_pointer = live_chart_config_ref (src_value->data[0].v_pointer);
#line 764 "config.c"
	} else {
#line 42 "../src/config.vala"
		dest_value->data[0].v_pointer = NULL;
#line 768 "config.c"
	}
}

static gpointer
live_chart_value_config_peek_pointer (const GValue* value)
{
#line 42 "../src/config.vala"
	return value->data[0].v_pointer;
#line 777 "config.c"
}

static gchar*
live_chart_value_config_collect_value (GValue* value,
                                       guint n_collect_values,
                                       GTypeCValue* collect_values,
                                       guint collect_flags)
{
#line 42 "../src/config.vala"
	if (collect_values[0].v_pointer) {
#line 788 "config.c"
		LiveChartConfig * object;
		object = collect_values[0].v_pointer;
#line 42 "../src/config.vala"
		if (object->parent_instance.g_class == NULL) {
#line 42 "../src/config.vala"
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
#line 795 "config.c"
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
#line 42 "../src/config.vala"
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
#line 799 "config.c"
		}
#line 42 "../src/config.vala"
		value->data[0].v_pointer = live_chart_config_ref (object);
#line 803 "config.c"
	} else {
#line 42 "../src/config.vala"
		value->data[0].v_pointer = NULL;
#line 807 "config.c"
	}
#line 42 "../src/config.vala"
	return NULL;
#line 811 "config.c"
}

static gchar*
live_chart_value_config_lcopy_value (const GValue* value,
                                     guint n_collect_values,
                                     GTypeCValue* collect_values,
                                     guint collect_flags)
{
	LiveChartConfig ** object_p;
	object_p = collect_values[0].v_pointer;
#line 42 "../src/config.vala"
	if (!object_p) {
#line 42 "../src/config.vala"
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
#line 826 "config.c"
	}
#line 42 "../src/config.vala"
	if (!value->data[0].v_pointer) {
#line 42 "../src/config.vala"
		*object_p = NULL;
#line 832 "config.c"
	} else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
#line 42 "../src/config.vala"
		*object_p = value->data[0].v_pointer;
#line 836 "config.c"
	} else {
#line 42 "../src/config.vala"
		*object_p = live_chart_config_ref (value->data[0].v_pointer);
#line 840 "config.c"
	}
#line 42 "../src/config.vala"
	return NULL;
#line 844 "config.c"
}

GParamSpec*
live_chart_param_spec_config (const gchar* name,
                              const gchar* nick,
                              const gchar* blurb,
                              GType object_type,
                              GParamFlags flags)
{
	LiveChartParamSpecConfig* spec;
#line 42 "../src/config.vala"
	g_return_val_if_fail (g_type_is_a (object_type, LIVE_CHART_TYPE_CONFIG), NULL);
#line 42 "../src/config.vala"
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
#line 42 "../src/config.vala"
	G_PARAM_SPEC (spec)->value_type = object_type;
#line 42 "../src/config.vala"
	return G_PARAM_SPEC (spec);
#line 863 "config.c"
}

gpointer
live_chart_value_get_config (const GValue* value)
{
#line 42 "../src/config.vala"
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, LIVE_CHART_TYPE_CONFIG), NULL);
#line 42 "../src/config.vala"
	return value->data[0].v_pointer;
#line 873 "config.c"
}

void
live_chart_value_set_config (GValue* value,
                             gpointer v_object)
{
	LiveChartConfig * old;
#line 42 "../src/config.vala"
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, LIVE_CHART_TYPE_CONFIG));
#line 42 "../src/config.vala"
	old = value->data[0].v_pointer;
#line 42 "../src/config.vala"
	if (v_object) {
#line 42 "../src/config.vala"
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, LIVE_CHART_TYPE_CONFIG));
#line 42 "../src/config.vala"
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
#line 42 "../src/config.vala"
		value->data[0].v_pointer = v_object;
#line 42 "../src/config.vala"
		live_chart_config_ref (value->data[0].v_pointer);
#line 895 "config.c"
	} else {
#line 42 "../src/config.vala"
		value->data[0].v_pointer = NULL;
#line 899 "config.c"
	}
#line 42 "../src/config.vala"
	if (old) {
#line 42 "../src/config.vala"
		live_chart_config_unref (old);
#line 905 "config.c"
	}
}

void
live_chart_value_take_config (GValue* value,
                              gpointer v_object)
{
	LiveChartConfig * old;
#line 42 "../src/config.vala"
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, LIVE_CHART_TYPE_CONFIG));
#line 42 "../src/config.vala"
	old = value->data[0].v_pointer;
#line 42 "../src/config.vala"
	if (v_object) {
#line 42 "../src/config.vala"
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, LIVE_CHART_TYPE_CONFIG));
#line 42 "../src/config.vala"
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
#line 42 "../src/config.vala"
		value->data[0].v_pointer = v_object;
#line 926 "config.c"
	} else {
#line 42 "../src/config.vala"
		value->data[0].v_pointer = NULL;
#line 930 "config.c"
	}
#line 42 "../src/config.vala"
	if (old) {
#line 42 "../src/config.vala"
		live_chart_config_unref (old);
#line 936 "config.c"
	}
}

static void
live_chart_config_class_init (LiveChartConfigClass * klass,
                              gpointer klass_data)
{
#line 42 "../src/config.vala"
	live_chart_config_parent_class = g_type_class_peek_parent (klass);
#line 42 "../src/config.vala"
	((LiveChartConfigClass *) klass)->finalize = live_chart_config_finalize;
#line 42 "../src/config.vala"
	g_type_class_adjust_private_offset (klass, &LiveChartConfig_private_offset);
#line 950 "config.c"
}

static void
live_chart_config_instance_init (LiveChartConfig * self,
                                 gpointer klass)
{
	LiveChartYAxis* _tmp0_;
	LiveChartXAxis* _tmp1_;
#line 42 "../src/config.vala"
	self->priv = live_chart_config_get_instance_private (self);
#line 44 "../src/config.vala"
	self->priv->_width = 0;
#line 48 "../src/config.vala"
	self->priv->_height = 0;
#line 52 "../src/config.vala"
	live_chart_padding_init (&self->padding);
#line 54 "../src/config.vala"
	_tmp0_ = live_chart_yaxis_new ("");
#line 54 "../src/config.vala"
	self->y_axis = _tmp0_;
#line 55 "../src/config.vala"
	_tmp1_ = live_chart_xaxis_new ();
#line 55 "../src/config.vala"
	self->x_axis = _tmp1_;
#line 42 "../src/config.vala"
	self->ref_count = 1;
#line 977 "config.c"
}

static void
live_chart_config_finalize (LiveChartConfig * obj)
{
	LiveChartConfig * self;
#line 42 "../src/config.vala"
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, LIVE_CHART_TYPE_CONFIG, LiveChartConfig);
#line 42 "../src/config.vala"
	g_signal_handlers_destroy (self);
#line 54 "../src/config.vala"
	_live_chart_yaxis_unref0 (self->y_axis);
#line 55 "../src/config.vala"
	_live_chart_xaxis_unref0 (self->x_axis);
#line 57 "../src/config.vala"
	_g_object_unref0 (self->categories);
#line 994 "config.c"
}

static GType
live_chart_config_get_type_once (void)
{
	static const GTypeValueTable g_define_type_value_table = { live_chart_value_config_init, live_chart_value_config_free_value, live_chart_value_config_copy_value, live_chart_value_config_peek_pointer, "p", live_chart_value_config_collect_value, "p", live_chart_value_config_lcopy_value };
	static const GTypeInfo g_define_type_info = { sizeof (LiveChartConfigClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) live_chart_config_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (LiveChartConfig), 0, (GInstanceInitFunc) live_chart_config_instance_init, &g_define_type_value_table };
	static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
	GType live_chart_config_type_id;
	live_chart_config_type_id = g_type_register_fundamental (g_type_fundamental_next (), "LiveChartConfig", &g_define_type_info, &g_define_type_fundamental_info, 0);
	LiveChartConfig_private_offset = g_type_add_instance_private (live_chart_config_type_id, sizeof (LiveChartConfigPrivate));
	return live_chart_config_type_id;
}

GType
live_chart_config_get_type (void)
{
	static volatile gsize live_chart_config_type_id__once = 0;
	if (g_once_init_enter (&live_chart_config_type_id__once)) {
		GType live_chart_config_type_id;
		live_chart_config_type_id = live_chart_config_get_type_once ();
		g_once_init_leave (&live_chart_config_type_id__once, live_chart_config_type_id);
	}
	return live_chart_config_type_id__once;
}

gpointer
live_chart_config_ref (gpointer instance)
{
	LiveChartConfig * self;
	self = instance;
#line 42 "../src/config.vala"
	g_atomic_int_inc (&self->ref_count);
#line 42 "../src/config.vala"
	return instance;
#line 1030 "config.c"
}

void
live_chart_config_unref (gpointer instance)
{
	LiveChartConfig * self;
	self = instance;
#line 42 "../src/config.vala"
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
#line 42 "../src/config.vala"
		LIVE_CHART_CONFIG_GET_CLASS (self)->finalize (self);
#line 42 "../src/config.vala"
		g_type_free_instance ((GTypeInstance *) self);
#line 1044 "config.c"
	}
}

