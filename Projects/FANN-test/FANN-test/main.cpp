#include "fann.h"


int main()
{
	const unsigned int num_input = 1;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 3;
	const unsigned int num_neurons_hidden = 3;
	const float desired_error = (const float) 0.00005;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 1000;

	struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	//fann_train_on_file(ann, "xor.data", max_epochs, epochs_between_reports, desired_error);

	/*fann_type input[5] = { 1, 2, 3, 4, 5 };
	fann_type output[5] = { 1, 4, 9, 16, 25 };

	fann_train_data *trainingData = fann_create_train(5, 5, 5);
	trainingData->input = input;
	trainingData->output = output;

	fann_train_on_data(ann, input, output, max_epochs, epochs_between_reports, desired_error);*/

	fann_train_on_file(ann, "test.data", max_epochs, epochs_between_reports, desired_error);

	//fann_save(ann, "xor_float.net");

	fann_type *calc_out;
	//fann_type input[2];

	//ann = fann_create_from_file("xor_float.net");

	fann_type testInput[1];

	testInput[0] = -1;
	calc_out = fann_run(ann, testInput);
	printf("power test (%f) -> %f\n", testInput[0], calc_out[0]);

	testInput[0] = -0.916666667 ;
	calc_out = fann_run(ann, testInput);
	printf("power test (%f) -> %f\n", testInput[0], calc_out[0]);

	testInput[0] = -0.833333333;
	calc_out = fann_run(ann, testInput);
	printf("power test (%f) -> %f\n", testInput[0], calc_out[0]);

	testInput[0] = -0.75;
	calc_out = fann_run(ann, testInput);
	printf("power test (%f) -> %f\n", testInput[0], calc_out[0]);

	testInput[0] = -0.666666667;
	calc_out = fann_run(ann, testInput);
	printf("power test (%f) -> %f\n", testInput[0], calc_out[0]);

	fann_destroy(ann);

	getchar();
	return 0;
}