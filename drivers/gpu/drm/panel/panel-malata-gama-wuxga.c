// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct malata_gama_wuxga {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *enable_gpio;
	bool prepared;
};

static inline
struct malata_gama_wuxga *to_malata_gama_wuxga(struct drm_panel *panel)
{
	return container_of(panel, struct malata_gama_wuxga, panel);
}

static void malata_gama_wuxga_reset(struct malata_gama_wuxga *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(30);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
}

static int malata_gama_wuxga_on(struct malata_gama_wuxga *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xbf, 0x04);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x00);

	return 0;
}

static int malata_gama_wuxga_panel_on(struct malata_gama_wuxga *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(80);

	return 0;
}

static int malata_gama_wuxga_off(struct malata_gama_wuxga *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off(dsi);
	msleep(80);

	mipi_dsi_dcs_enter_sleep_mode(dsi);
	msleep(120);

	return 0;
}

static int malata_gama_wuxga_prepare(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	gpiod_set_value_cansleep(ctx->enable_gpio, 1);
	msleep(50);

	malata_gama_wuxga_reset(ctx);

	ret = malata_gama_wuxga_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int malata_gama_wuxga_enable(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = malata_gama_wuxga_panel_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to turn on panel: %d\n", ret);
		return ret;
	}
	return 0;
}

static int malata_gama_wuxga_unprepare(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = malata_gama_wuxga_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->enable_gpio, 0);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static int malata_gama_wuxga_disable(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = malata_gama_wuxga_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	return 0;
}

static const struct drm_display_mode malata_gama_wuxga_mode = {
	.clock = (1200 + 60 + 4 + 80) * (1920 + 34 + 2 + 24) * 60 / 1000,
	.hdisplay = 1200,
	.hsync_start = 1200 + 60,
	.hsync_end = 1200 + 60 + 4,
	.htotal = 1200 + 60 + 4 + 80,
	.vdisplay = 1920,
	.vsync_start = 1920 + 34,
	.vsync_end = 1920 + 34 + 2,
	.vtotal = 1920 + 34 + 2 + 24,
	.width_mm = 135,
	.height_mm = 216,
};

static int malata_gama_wuxga_get_modes(struct drm_panel *panel,
				       struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &malata_gama_wuxga_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs malata_gama_wuxga_panel_funcs = {
	.disable = malata_gama_wuxga_disable,
	.enable = malata_gama_wuxga_enable,
	.prepare = malata_gama_wuxga_prepare,
	.unprepare = malata_gama_wuxga_unprepare,
	.get_modes = malata_gama_wuxga_get_modes,
};

static int malata_gama_wuxga_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct malata_gama_wuxga *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->enable_gpio = devm_gpiod_get(dev, "enable", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->enable_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->enable_gpio),
				     "Failed to get enable-gpios\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
		      MIPI_DSI_MODE_LPM ;

	drm_panel_init(&ctx->panel, dev, &malata_gama_wuxga_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static void malata_gama_wuxga_remove(struct mipi_dsi_device *dsi)
{
	struct malata_gama_wuxga *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id malata_gama_wuxga_of_match[] = {
	{ .compatible = "malata,gama-wuxga" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, malata_gama_wuxga_of_match);

static struct mipi_dsi_driver malata_gama_wuxga_driver = {
	.probe = malata_gama_wuxga_probe,
	.remove = malata_gama_wuxga_remove,
	.driver = {
		.name = "panel-malata-gama-wuxga",
		.of_match_table = malata_gama_wuxga_of_match,
	},
};
module_mipi_dsi_driver(malata_gama_wuxga_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for malata 1200 1920 video mode dsi panel");
MODULE_LICENSE("GPL");
