// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct nt35521s_ebbg {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
};

static inline struct nt35521s_ebbg *to_nt35521s_ebbg(struct drm_panel *panel)
{
	return container_of(panel, struct nt35521s_ebbg, panel);
}

static void nt35521s_ebbg_reset(struct nt35521s_ebbg *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int nt35521s_ebbg_on(struct nt35521s_ebbg *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf0,
					 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xb1, 0x6c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc8, 0x83);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x6f, 0x02);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xb2, 0x02);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff,
					 0xaa, 0x55, 0x25, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x6f, 0x10);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf7, 0x1d);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x6f, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf7, 0x40);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff,
					 0xaa, 0x55, 0x25, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x35, 0x00);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 32);

	return dsi_ctx.accum_err;
}

static int nt35521s_ebbg_off(struct nt35521s_ebbg *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int nt35521s_ebbg_prepare(struct drm_panel *panel)
{
	struct nt35521s_ebbg *ctx = to_nt35521s_ebbg(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	nt35521s_ebbg_reset(ctx);

	ret = nt35521s_ebbg_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	return 0;
}

static int nt35521s_ebbg_unprepare(struct drm_panel *panel)
{
	struct nt35521s_ebbg *ctx = to_nt35521s_ebbg(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = nt35521s_ebbg_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	return 0;
}

static const struct drm_display_mode nt35521s_ebbg_mode = {
	.clock = (720 + 200 + 20 + 200) * (1280 + 35 + 2 + 28) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 200,
	.hsync_end = 720 + 200 + 20,
	.htotal = 720 + 200 + 20 + 200,
	.vdisplay = 1280,
	.vsync_start = 1280 + 35,
	.vsync_end = 1280 + 35 + 2,
	.vtotal = 1280 + 35 + 2 + 28,
	.width_mm = 62,
	.height_mm = 110,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int nt35521s_ebbg_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &nt35521s_ebbg_mode);
}

static const struct drm_panel_funcs nt35521s_ebbg_panel_funcs = {
	.prepare = nt35521s_ebbg_prepare,
	.unprepare = nt35521s_ebbg_unprepare,
	.get_modes = nt35521s_ebbg_get_modes,
};

static int nt35521s_ebbg_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct nt35521s_ebbg *ctx;
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
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS |
			  MIPI_DSI_MODE_VIDEO_NO_HBP | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &nt35521s_ebbg_panel_funcs,
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

static void nt35521s_ebbg_remove(struct mipi_dsi_device *dsi)
{
	struct nt35521s_ebbg *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id nt35521s_ebbg_of_match[] = {
	{ .compatible = "xiaomi,land-nt35521s-ebbg" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, nt35521s_ebbg_of_match);

static struct mipi_dsi_driver nt35521s_ebbg_driver = {
	.probe = nt35521s_ebbg_probe,
	.remove = nt35521s_ebbg_remove,
	.driver = {
		.name = "panel-xiaomi-land-nt35521s-ebbg",
		.of_match_table = nt35521s_ebbg_of_match,
	},
};
module_mipi_dsi_driver(nt35521s_ebbg_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for nt35521s_HD720p_video_EBBG");
MODULE_LICENSE("GPL");
