// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct malata_gama_wuxga {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *enable_gpio;
	//struct gpio_desc *blen_gpio;
};

static const struct regulator_bulk_data malata_gama_wuxga_supplies[] = {
	{ .supply = "vdd" },
	{ .supply = "vddio" },
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
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;
	msleep(24);

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbf, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0x00);

	mipi_dsi_msleep(&ctx, 150);

	return dsi_ctx.accum_err;
}

/*static int malata_gama_wuxga_off(struct malata_gama_wuxga *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 100);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}*/

static int malata_gama_wuxga_prepare(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(malata_gama_wuxga_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	gpiod_set_value_cansleep(ctx->enable_gpio, 1);
	usleep_range(10000, 15000);

	ret = mipi_dsi_dcs_nop(ctx->dsi);
	if (ret < 0) {
			dev_err(dev, "Failed to send NOP: %d\n", ret);
	}
	usleep_range(1000, 2000);

	malata_gama_wuxga_reset(ctx);

	ret = malata_gama_wuxga_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(malata_gama_wuxga_supplies), ctx->supplies);
		return ret;
	}
	/*gpiod_set_value_cansleep(ctx->blen_gpio, 1);
	msleep(200);*/

	return 0;
}

static int malata_gama_wuxga_enable(struct drm_panel *panel)
{
	//struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	//struct device *dev = &ctx->dsi->dev;

	/*gpiod_set_value_cansleep(ctx->blen_gpio, 1);*/
	msleep(200);

	return 0;
}

static int malata_gama_wuxga_unprepare(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	/*ret = malata_gama_wuxga_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);*/

	/*gpiod_set_value_cansleep(ctx->blen_gpio, 0);
	msleep(100);*/

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 3000);

	gpiod_set_value_cansleep(ctx->enable_gpio, 0);
	usleep_range(2000, 4000);

	regulator_bulk_disable(ARRAY_SIZE(malata_gama_wuxga_supplies), ctx->supplies);
	usleep_range(7000, 9000);

	return 0;
}

static int malata_gama_wuxga_disable(struct drm_panel *panel)
{
	struct malata_gama_wuxga *ctx = to_malata_gama_wuxga(panel);
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };
	//struct device *dev = &ctx->dsi->dev;

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 100);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
//	gpiod_set_value_cansleep(ctx->blen_gpio, 0);
//	msleep(100);

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
	.type = DRM_MODE_TYPE_DRIVER,
};

static int malata_gama_wuxga_get_modes(struct drm_panel *panel,
				       struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &malata_gama_wuxga_mode);
}

static const struct drm_panel_funcs malata_gama_wuxga_panel_funcs = {
	.prepare = malata_gama_wuxga_prepare,
	.enable = malata_gama_wuxga_enable,
	.disable = malata_gama_wuxga_disable,
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

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(malata_gama_wuxga_supplies),
					    malata_gama_wuxga_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	/*ctx->blen_gpio = devm_gpiod_get(dev, "blen", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->blen_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->blen_gpio),
				     "Failed to get backlight-enable-gpios\n");*/

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
		      MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &malata_gama_wuxga_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
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
