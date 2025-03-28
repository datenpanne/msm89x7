// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct tianma_521_v0 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
};

static inline struct tianma_521_v0 *to_tianma_521_v0(struct drm_panel *panel)
{
	return container_of(panel, struct tianma_521_v0, panel);
}

static void tianma_521_v0_reset(struct tianma_521_v0 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int tianma_521_v0_on(struct tianma_521_v0 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xb0, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xd6, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc2,
					 0x31, 0xf7, 0x80, 0x0a, 0x06, 0x00,
					 0x00, 0x08);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xd3,
					 0x1b, 0x33, 0xbb, 0xbb);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xce,
					 0x24, 0x40, 0x49, 0x53, 0x59, 0x5e,
					 0x63, 0x68, 0x6e, 0x74, 0x7e, 0x8a,
					 0x98, 0xa8, 0xbb, 0xd0, 0xff, 0x04,
					 0x00, 0x02, 0x02);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xb0, 0x03);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_set_display_brightness_multi(&dsi_ctx, 0x00cc);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     0x2c);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_POWER_SAVE, 0x01);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 80);

	return dsi_ctx.accum_err;
}

static int tianma_521_v0_off(struct tianma_521_v0 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 80);

	return dsi_ctx.accum_err;
}

static int tianma_521_v0_prepare(struct drm_panel *panel)
{
	struct tianma_521_v0 *ctx = to_tianma_521_v0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	tianma_521_v0_reset(ctx);

	ret = tianma_521_v0_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	return 0;
}

static int tianma_521_v0_unprepare(struct drm_panel *panel)
{
	struct tianma_521_v0 *ctx = to_tianma_521_v0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = tianma_521_v0_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	return 0;
}

static const struct drm_display_mode tianma_521_v0_mode = {
	.clock = (1080 + 120 + 20 + 120) * (1920 + 14 + 2 + 8) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 120,
	.hsync_end = 1080 + 120 + 20,
	.htotal = 1080 + 120 + 20 + 120,
	.vdisplay = 1920,
	.vsync_start = 1920 + 14,
	.vsync_end = 1920 + 14 + 2,
	.vtotal = 1920 + 14 + 2 + 8,
	.width_mm = 65,
	.height_mm = 115,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int tianma_521_v0_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &tianma_521_v0_mode);
}

static const struct drm_panel_funcs tianma_521_v0_panel_funcs = {
	.prepare = tianma_521_v0_prepare,
	.unprepare = tianma_521_v0_unprepare,
	.get_modes = tianma_521_v0_get_modes,
};

static int tianma_521_v0_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tianma_521_v0 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &tianma_521_v0_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void tianma_521_v0_remove(struct mipi_dsi_device *dsi)
{
	struct tianma_521_v0 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id tianma_521_v0_of_match[] = {
	{ .compatible = "motorola,montana-r63350-tianma" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tianma_521_v0_of_match);

static struct mipi_dsi_driver tianma_521_v0_driver = {
	.probe = tianma_521_v0_probe,
	.remove = tianma_521_v0_remove,
	.driver = {
		.name = "panel-motorola-montana-r63350-tianma",
		.of_match_table = tianma_521_v0_of_match,
	},
};
module_mipi_dsi_driver(tianma_521_v0_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for mipi_mot_vid_tianma_1080p_521");
MODULE_LICENSE("GPL");
