#include "HetrickCV.hpp"

struct Exponent : Module 
{
	enum ParamIds 
	{
		AMOUNT_PARAM,
        SCALE_PARAM,
        RANGE_PARAM,
		NUM_PARAMS
	};
	enum InputIds 
	{
        MAIN_INPUT,
        AMOUNT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds 
	{
		MAIN_OUTPUT,
		NUM_OUTPUTS
	};

	Exponent() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) 
	{
		
	}

	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - reset, randomize: implements special behavior when user clicks these from the context menu
};


void Exponent::step() 
{
	float input = inputs[MAIN_INPUT].value;
	const bool negativeInput = input < 0.0f;

    bool mode5V = (params[RANGE_PARAM].value == 0.0f);
    if(mode5V) input = clampf(input, -5.0f, 5.0f) * 0.2f;
	else input = clampf(input, -10.0f, 10.0f) * 0.1f;
	input = std::abs(input);

    float exponent = params[AMOUNT_PARAM].value + (inputs[AMOUNT_INPUT].value * params[SCALE_PARAM].value);
    exponent = clampf(exponent, -5.0f, 5.0f) * 0.2f;

	if(exponent < 0)
	{
		exponent = 1.0f - (exponent * -0.5f);
	}
	else exponent += 1.0f;

    float output = powf(input, exponent);

	if (negativeInput) output *= -1.0f;
    if(mode5V) output *= 5.0f;
    else output *= 10.0f;
    
    outputs[MAIN_OUTPUT].value = output;
}

struct CKSSRot : SVGSwitch, ToggleSwitch {
	CKSSRot() {
		addFrame(SVG::load(assetPlugin(plugin, "res/CKSS_rot_0.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/CKSS_rot_1.svg")));
		sw->wrap();
		box.size = sw->box.size;
	}
};

ExponentWidget::ExponentWidget() 
{
	auto *module = new Exponent();
	setModule(module);
	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		auto *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Exponent.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 365)));

	//////PARAMS//////
	addParam(createParam<Davies1900hBlackKnob>(Vec(27, 62), module, Exponent::AMOUNT_PARAM, -5.0, 5.0, 0.0));
    addParam(createParam<Trimpot>(Vec(36, 112), module, Exponent::SCALE_PARAM, -1.0, 1.0, 1.0));
    addParam(createParam<CKSSRot>(Vec(35, 200), module, Exponent::RANGE_PARAM, 0.0, 1.0, 0.0));

	//////INPUTS//////
    addInput(createInput<PJ301MPort>(Vec(33, 235), module, Exponent::MAIN_INPUT));
    addInput(createInput<PJ301MPort>(Vec(33, 145), module, Exponent::AMOUNT_INPUT));

	//////OUTPUTS//////
	addOutput(createOutput<PJ301MPort>(Vec(33, 285), module, Exponent::MAIN_OUTPUT));
}
