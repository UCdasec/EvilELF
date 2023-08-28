#!/bin/sh
python train_models.py --model_name=malconv --input_size=100000 --window_size=500 --batch_size=32 --epochs=50 --lr=0.0001 --num_workers=1 --log_file_path=../models/malconv_100k_even-split_0001_500_32_model.txt --train_label_path=../data/TrainEven_ELF_Dataset.csv --test_label_path=../data/TestEven_ELF_Dataset.csv --val_label_path=../data/ValidEven_ELF_Dataset.csv &&
mv ../checkpoint/malconv_model.pth ../models/malconv_100k_even-split_0001_500_32_model.pth &&
python train_models.py --model_name=malconv --input_size=200000 --window_size=500 --batch_size=32 --epochs=50 --lr=0.0001 --num_workers=1 --log_file_path=../models/malconv_200k_even-split_0001_500_32_model.txt --train_label_path=../data/TrainEven_ELF_Dataset.csv --test_label_path=../data/TestEven_ELF_Dataset.csv --val_label_path=../data/ValidEven_ELF_Dataset.csv &&
mv ../checkpoint/malconv_model.pth ../models/malconv_200k_even-split_0001_500_32_model.pth &&
python train_models.py --model_name=malconv --input_size=500000 --window_size=500 --batch_size=32 --epochs=50 --lr=0.0001 --num_workers=1 --log_file_path=../models/malconv_500k_even-split_0001_500_32_model.txt --train_label_path=../data/TrainEven_ELF_Dataset.csv --test_label_path=../data/TestEven_ELF_Dataset.csv --val_label_path=../data/ValidEven_ELF_Dataset.csv &&
mv ../checkpoint/malconv_model.pth ../models/malconv_500k_even-split_0001_500_32_model.pth &&
python train_models.py --model_name=malconv --input_size=1000000 --window_size=500 --batch_size=32 --epochs=50 --lr=0.0001 --num_workers=1 --log_file_path=../models/malconv_1000k_even-split_0001_500_32_model.txt --train_label_path=../data/TrainEven_ELF_Dataset.csv --test_label_path=../data/TestEven_ELF_Dataset.csv --val_label_path=../data/ValidEven_ELF_Dataset.csv &&
mv ../checkpoint/malconv_model.pth ../models/malconv_1000k_even-split_0001_500_32_model.pth 