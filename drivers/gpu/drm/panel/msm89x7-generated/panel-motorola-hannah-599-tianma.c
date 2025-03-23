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

struct tianma_599_v0 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
};

static inline struct tianma_599_v0 *to_tianma_599_v0(struct drm_panel *panel)
{
	return container_of(panel, struct tianma_599_v0, panel);
}

static void tianma_599_v0_reset(struct tianma_599_v0 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int tianma_599_v0_on(struct tianma_599_v0 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x23);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x08, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x24);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0b, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x0c, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x16, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x21, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x22, 0x20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x10);
	mipi_dsi_dcs_set_display_brightness_multi(&dsi_ctx, 0x00cc);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_CONTROL_DISPLAY,
				     0x2c);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_POWER_SAVE, 0x01);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);

	return dsi_ctx.accum_err;
}

static int tianma_599_v0_off(struct tianma_599_v0 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int tianma_599_v0_prepare(struct drm_panel *panel)
{
	struct tianma_599_v0 *ctx = to_tianma_599_v0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	tianma_599_v0_reset(ctx);

	ret = tianma_599_v0_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	return 0;
}

static int tianma_599_v0_unprepare(struct drm_panel *panel)
{
	struct tianma_599_v0 *ctx = to_tianma_599_v0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = tianma_599_v0_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	return 0;
}

static const struct drm_display_mode tianma_599_v0_mode = {
	.clock = (720 + 364 + 20 + 40) * (1440 + 10 + 2 + 8) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 364,
	.hsync_end = 720 + 364 + 20,
	.htotal = 720 + 364 + 20 + 40,
	.vdisplay = 1440,
	.vsync_start = 1440 + 10,
	.vsync_end = 1440 + 10 + 2,
	.vtotal = 1440 + 10 + 2 + 8,
	.width_mm = 68,
	.height_mm = 136,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int tianma_599_v0_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &tianma_599_v0_mode);
}

static const struct drm_panel_funcs tianma_599_v0_panel_funcs = {
	.prepare = tianma_599_v0_prepare,
	.unprepare = tianma_599_v0_unprepare,
	.get_modes = tianma_599_v0_get_modes,
};

static int tianma_599_v0_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tianma_599_v0 *ctx;
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
			  MIPI_DSI_CLOCK_NON_CONTINUOUS |
			  MIPI_DSI_MODE_VIDEO_NO_HFP | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &tianma_599_v0_panel_funcs,
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

static void tianma_599_v0_remove(struct mipi_dsi_device *dsi)
{
	struct tianma_599_v0 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id tianma_599_v0_of_match[] = {
	{ .compatible = "motorola,hannah-599-tianma" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tianma_599_v0_of_match);

static struct mipi_dsi_driver tianma_599_v0_driver = {
	.probe = tianma_599_v0_probe,
	.remove = tianma_599_v0_remove,
	.driver = {
		.name = "panel-motorola-hannah-599-tianma",
		.of_match_table = tianma_599_v0_of_match,
	},
};
module_mipi_dsi_driver(tianma_599_v0_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for mipi_mot_video_tianma_hd_599");
MODULE_LICENSE("GPL");
