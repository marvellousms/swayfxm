#include <string.h>
#include <strings.h>
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "log.h"
#include "util.h"

static struct cmd_results *cmd_titlebar_buttons_enable_disable(int argc, char **argv) {
	if (argc < 1) {
		return cmd_results_new(CMD_INVALID, "Expected 'titlebar_buttons enable|disable'");
	}
	
	bool enable = parse_boolean(argv[0], false);
	config->titlebar_buttons.enabled = enable;
	
	// Rearrange all containers to apply changes
	arrange_root();
	
	return cmd_results_new(CMD_SUCCESS, NULL);
}

static struct cmd_results *cmd_titlebar_buttons_size(int argc, char **argv) {
	if (argc < 1) {
		return cmd_results_new(CMD_INVALID, "Expected 'titlebar_buttons size <pixels>'");
	}
	
	int size = atoi(argv[0]);
	if (size < 4 || size > 50) {
		return cmd_results_new(CMD_INVALID, "Size must be between 4 and 50 pixels");
	}
	
	config->titlebar_buttons.size = size;
	arrange_root();
	
	return cmd_results_new(CMD_SUCCESS, NULL);
}

static struct cmd_results *cmd_titlebar_buttons_padding(int argc, char **argv) {
	if (argc < 1) {
		return cmd_results_new(CMD_INVALID, "Expected 'titlebar_buttons padding <pixels>'");
	}
	
	int padding = atoi(argv[0]);
	if (padding < 0 || padding > 50) {
		return cmd_results_new(CMD_INVALID, "Padding must be between 0 and 50 pixels");
	}
	
	config->titlebar_buttons.padding = padding;
	arrange_root();
	
	return cmd_results_new(CMD_SUCCESS, NULL);
}

static struct cmd_results *cmd_titlebar_buttons_position(int argc, char **argv) {
	if (argc < 1) {
		return cmd_results_new(CMD_INVALID, "Expected 'titlebar_buttons position left|right'");
	}
	
	if (strcasecmp(argv[0], "left") == 0) {
		config->titlebar_buttons.position = BUTTONS_LEFT;
	} else if (strcasecmp(argv[0], "right") == 0) {
		config->titlebar_buttons.position = BUTTONS_RIGHT;
	} else {
		return cmd_results_new(CMD_INVALID, "Position must be 'left' or 'right'");
	}
	
	arrange_root();
	
	return cmd_results_new(CMD_SUCCESS, NULL);
}

static struct cmd_results *cmd_titlebar_buttons_colors(int argc, char **argv) {
	if (argc < 3) {
		return cmd_results_new(CMD_INVALID,
			"Expected 'titlebar_buttons colors <button> <state> <color>'");
	}

	const char *button = argv[0];
	const char *state = argv[1];
	const char *color_str = argv[2];

	float *color_ptr = NULL;

	// Select which color to modify
	if (strcasecmp(button, "close") == 0) {
		if (strcasecmp(state, "normal") == 0) {
			color_ptr = config->titlebar_buttons.close_color;
		} else if (strcasecmp(state, "hover") == 0) {
			color_ptr = config->titlebar_buttons.close_hover;
		} else if (strcasecmp(state, "pressed") == 0) {
			color_ptr = config->titlebar_buttons.close_pressed;
		}
	} else if (strcasecmp(button, "minimize") == 0) {
		if (strcasecmp(state, "normal") == 0) {
			color_ptr = config->titlebar_buttons.minimize_color;
		} else if (strcasecmp(state, "hover") == 0) {
			color_ptr = config->titlebar_buttons.minimize_hover;
		} else if (strcasecmp(state, "pressed") == 0) {
			color_ptr = config->titlebar_buttons.minimize_pressed;
		}
	} else if (strcasecmp(button, "maximize") == 0) {
		if (strcasecmp(state, "normal") == 0) {
			color_ptr = config->titlebar_buttons.maximize_color;
		} else if (strcasecmp(state, "hover") == 0) {
			color_ptr = config->titlebar_buttons.maximize_hover;
		} else if (strcasecmp(state, "pressed") == 0) {
			color_ptr = config->titlebar_buttons.maximize_pressed;
		}
	}

	if (!color_ptr) {
		return cmd_results_new(CMD_INVALID,
			"Invalid button or state. Use: close|minimize|maximize and normal|hover|pressed");
	}

	uint32_t color;
	if (!parse_color(color_str, &color)) {
		return cmd_results_new(CMD_INVALID, "Invalid color format");
	}
	color_to_rgba(color_ptr, color);

	arrange_root();

	return cmd_results_new(CMD_SUCCESS, NULL);
}

static struct cmd_results *cmd_titlebar_buttons_icons(int argc, char **argv) {
	if (argc < 2) {
		return cmd_results_new(CMD_INVALID, 
			"Expected 'titlebar_buttons icons <button> <svg_path>'");
	}
	
	const char *button = argv[0];
	const char *svg_path = argv[1];
	
	char **path_ptr = NULL;
	
	if (strcasecmp(button, "close") == 0) {
		path_ptr = &config->titlebar_buttons.close_svg;
	} else if (strcasecmp(button, "minimize") == 0) {
		path_ptr = &config->titlebar_buttons.minimize_svg;
	} else if (strcasecmp(button, "maximize") == 0) {
		path_ptr = &config->titlebar_buttons.maximize_svg;
	}
	
	if (!path_ptr) {
		return cmd_results_new(CMD_INVALID, "Invalid button. Use: close|minimize|maximize");
	}
	
	// Free old path if exists
	free(*path_ptr);
	
	// "none" clears the icon
	if (strcasecmp(svg_path, "none") == 0) {
		*path_ptr = NULL;
	} else {
		*path_ptr = strdup(svg_path);
	}
	
	arrange_root();
	
	return cmd_results_new(CMD_SUCCESS, NULL);
}

struct cmd_results *cmd_titlebar_buttons(int argc, char **argv) {
	if (argc < 1) {
		return cmd_results_new(CMD_INVALID,
			"Expected 'titlebar_buttons <enable|disable|size|padding|position|colors|icons> ...'");
	}
	
	const char *subcommand = argv[0];
	argc--; argv++;
	
	if (strcasecmp(subcommand, "enable") == 0 || 
			strcasecmp(subcommand, "disable") == 0) {
		char *args[] = { (char*)subcommand };
		return cmd_titlebar_buttons_enable_disable(1, args);
	} else if (strcasecmp(subcommand, "size") == 0) {
		return cmd_titlebar_buttons_size(argc, argv);
	} else if (strcasecmp(subcommand, "padding") == 0) {
		return cmd_titlebar_buttons_padding(argc, argv);
	} else if (strcasecmp(subcommand, "position") == 0) {
		return cmd_titlebar_buttons_position(argc, argv);
	} else if (strcasecmp(subcommand, "colors") == 0) {
		return cmd_titlebar_buttons_colors(argc, argv);
	} else if (strcasecmp(subcommand, "icons") == 0) {
		return cmd_titlebar_buttons_icons(argc, argv);
	}
	
	return cmd_results_new(CMD_INVALID, 
		"Unknown subcommand: '%s'. Use: enable, disable, size, padding, position, colors, or icons",
		subcommand);
}
