import pandas as pd
import argparse
import os
import math
import numpy as np

#python3 dataframe_creator.py --benignware ../Labeled-Elfs-main/benignware/ --malware ../Labeled-Elfs-main/malware

def read_directory(directory_path, is_malware):
    files = os.listdir(directory_path)
    list_df = []
    for file in files:
        file_path = os.path.join(directory_path, file)
        if os.path.isfile(file_path):
            row = create_file_row(file_path, is_malware) 
            list_df.append(row)
    return pd.DataFrame(data = list_df, columns=['name','is_malware','size','architecture','address_size','endianness','ABI','compiler_used','optimization_level','obfuscation','stripped','package','program'])

def create_file_row(file_path, is_malware):
    row = []
    base_name = os.path.basename(file_path)
    size = os.path.getsize(file_path)
    split = base_name.split("__")
    row.append(base_name)
    row.append(is_malware)
    row.append(size)
    row.extend(split)
    return row

def reduce_non_malware_to_same_amount(dataframe):
    malware_total = dataframe['is_malware'].sum()
    count = 0
    to_drop = []
    for i,row in dataframe.iterrows():
        if row['is_malware'] == 0:
            if count < malware_total:
                count = count + 1
            else:
                to_drop.append(i)

    return dataframe.drop(index=to_drop, axis = 0).sample(frac=1)


def main():
    parser = argparse.ArgumentParser("dataframe_creator")
    parser.add_argument("--benignware", help = "The path to a folder containing only benign ELF executables")
    parser.add_argument("--malware", help = "The path to a folder containing only malware ELF executables")
    parser.add_argument("--train_amount", default = 0.75, help = "Decimal fraction out of 1 to use for train split")
    parser.add_argument("--test_amount", default = 0.15, help = "Decimal fraction out of 1 to use for test split")
    parser.add_argument("--valid_amount", default = 0.1, help = "Decimal fraction out of 1 to use for validation split")
    parser.add_argument("--num_to_use", help = "Number of entries from files to use, leave empty for whole dataset")

    args = parser.parse_args()

    df1 = read_directory(args.benignware,0)
    df2 = read_directory(args.malware,1)

    combined_df = pd.concat([df1, df2], ignore_index=True)

    combined_df = combined_df.sample(frac = 1,random_state=42069,ignore_index = True)
    print(combined_df)
    combined_df.to_csv("Combined_ELF_Dataset.csv",index=False)

    combined_df = combined_df[~combined_df['program'].astype('str').str.endswith(".o")]

    if args.num_to_use is None:
        num_rows = combined_df.shape[0]
        df_to_split = combined_df
    else:
        num_rows = int(args.num_to_use )
        df_to_split = combined_df.iloc[:num_rows]

    print(df_to_split)
    print("Malware amount: ", df_to_split['is_malware'].sum())
    
    train, test, valid = np.split(df_to_split, [int(args.train_amount*num_rows), int((args.train_amount+args.test_amount)*num_rows)])

    train.to_csv("Train_ELF_Dataset.csv",index=False)
    malware_amount = train['is_malware'].sum()
    print("Train Benignware amount: " + str( train.shape[0] - malware_amount) + "\t\t Malware amount: " + str(malware_amount))

    test.to_csv("Test_ELF_Dataset.csv",index=False)
    malware_amount = test['is_malware'].sum()
    print("Test Benignware amount: " + str( test.shape[0] - malware_amount) + "\t\t Malware amount: " + str(malware_amount))

    valid.to_csv("Valid_ELF_Dataset.csv",index=False)
    malware_amount = valid['is_malware'].sum()
    print("Validation Benignware amount: " + str( valid.shape[0] - malware_amount) + "\t\t Malware amount: " + str(malware_amount))

    reduce_non_malware_to_same_amount(train).to_csv("TrainEven_ELF_Dataset.csv")
    reduce_non_malware_to_same_amount(test).to_csv("TestEven_ELF_Dataset.csv")
    reduce_non_malware_to_same_amount(valid).to_csv("ValidEven_ELF_Dataset.csv")

    

    

if __name__ == "__main__":
    main()