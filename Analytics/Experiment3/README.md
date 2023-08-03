Generate malware by running experiment3_generate_malware.py with the correct os.command() uncommented depending on whether you want to insert constants or sections from benign files into your malware. You will need to additionally uncomment the section in experiment3_support_constants.c or experiment3_support.c you wish to use for your modification. There is a makefile to make it easy to compile them.

Use create_malware_csv_from_directory to automatically convert all malware in your ModifiedElfOutput directory into a csv to pass to a deep learning model in MultiEvasion.

Copy the csv and malware into the MultiEvasion/data/experiment3 folder and remove any preexisting files in it.

Run the experiment3_train_script.sh script in MultiEvasion/src to automatically run test_model_simple.py to test the modified malware on all models directly in the MultiEvasion/models folder.