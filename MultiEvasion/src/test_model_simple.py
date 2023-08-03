"""
test the performance of malware detector
get acc, auc, f1, recall, precision, etc
get roc curve
"""

import os
os.sys.path.append('..')

import torch
from sklearn.metrics import classification_report,confusion_matrix,accuracy_score,roc_curve,auc
import pandas as pd
from src.model import MalConv,FireEye,AvastNet
from src.util import ExeDataset
from torch.utils.data import DataLoader
import argparse, matplotlib
import matplotlib.pyplot as plt

## avoid type 3 font in plot, use 42 instead
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42


class test_model:
    def __init__(self,model_path,model_type):
        self.model_path = model_path
        self.model_type = model_type
        self.model_name = os.path.basename(model_path)
        # self.model_path = '../checkpoint/../models/%s' % os.path.basename(output_file_name)
        self.device = ('cuda:0' if torch.cuda.is_available() else 'cpu')
        print('CUDA AVAILABLE: ', torch.cuda.is_available())
        print('test model path: %s' % self.model_path)



    def binary_acc(self,preds, y):

        # round predictions to cloest integer; sigmoid -->[0,1]
        # rounded_preds = torch.round(torch.sigmoid(preds))
        correct = (preds == y).float()
        acc = correct.sum() / len(correct)

        return acc


    def testing(self):
        "load data"
        test_label_path = args.test_label_path
        test_data_path = args.test_data_path

        input_size_string = self.model_name.split("_")[1].lower()
        if 'k' in input_size_string:
            input_size = int(input_size_string.replace('k',''))*1000
        else:
            input_size = int(input_size_string)
        

        # data pre-process
        test_label_table = pd.read_csv(test_label_path, header=None, index_col=0)
        
        test_label_table = test_label_table.rename(columns={1: 'ground_truth'})

        # data loader
        test_loader = DataLoader(
            ExeDataset(list(test_label_table.index), test_data_path, list(test_label_table.ground_truth), input_size),
            batch_size=args.batch_size, shuffle=False, num_workers=args.use_cpu)


        "load model"
        if self.model_type == 'MalConv':
            model = MalConv(max_input_size=input_size).to(self.device)
        elif self.model_type == 'FireEye':
            model = FireEye(input_length=input_size).to(self.device)
        elif self.model_type == 'AvastNet':
            model = AvastNet()
        else:
            print('choose the right model between MalConv and FireEye.')

        model.load_state_dict(torch.load(self.model_path,map_location=self.device))
        model.to(self.device)
        model.eval()

        "testing all samples"
        # accuracy = 0
        y_pred,y_test, preds_all = [],[],[]
        for i, data in enumerate(test_loader):
            test_x, test_y = data
            test_x, test_y = test_x.to(self.device), test_y.to(self.device)
            pred_lab = model(test_x)
            preds_all += (pred_lab[:,1].cpu().detach().numpy().squeeze().tolist())

            "binary for two values"
            preds = torch.argmax(pred_lab,1)
            y_test += (test_y.cpu().detach().numpy().squeeze().tolist())
            y_pred += (preds.cpu().detach().numpy().tolist())

        test_label_table['Prediction'] = y_pred
        test_label_table.to_csv(test_label_path[0:-4]+"__"+os.path.basename(self.model_path)[0:-4]+"__results.csv")
        print('confusion matrix:')
        print(confusion_matrix(y_test, y_pred))

        print(f"classification report: ".format(classification_report(y_test, y_pred,labels=[0,1])))
        print(classification_report(y_test, y_pred,labels=[0,1]))
        print(f'accuracy is {accuracy_score(y_test, y_pred)}')
        fpr, tpr, thresholds = roc_curve(y_test, preds_all, pos_label=1)
        print("Fpr: \n",fpr)
        print("Tpr: \n",tpr)

        auc_value = auc(fpr, tpr)
        print(f'AUC is {auc_value}')
        with open(args.result_path + self.model_name[0:-4] + "_result.txt",'w') as f:
            print('--------> testing results <---------', file=f)
            print('confusion matrix:',file=f)
            print(confusion_matrix(y_test, y_pred),file=f)
            print(f"classification report: ".format(classification_report(y_test, y_pred, labels=[0, 1])),file=f)
            print(classification_report(y_test, y_pred, labels=[0, 1]),file=f)
            print(f'accuracy is {accuracy_score(y_test, y_pred)}',file=f)
            print(f'AUC is {auc_value}',file=f)


        # plot roc curve
        font_size = 18
        plt.figure(figsize=(8, 6), dpi=150)
        plt.plot(fpr, tpr, color='red', linestyle='-', linewidth=2,
                 fillstyle='none',  label='ROC curve (area = %.3f)' % auc_value)
        plt.xlabel('False Positive Rate', {'size': font_size})
        plt.ylabel('True Positive Rate', {'size': font_size})
        plt.tick_params(labelsize=font_size)
        plt.legend(loc='best', fontsize=font_size)
        plt.savefig(args.result_path + self.model_name[0:-4]  + '_roc_curve.eps')
        plt.show()


def get_model_type(file_name):
    lower_file_name = file_name.lower()
    if "malconv" in lower_file_name:
        return "MalConv"
    elif "fireeye" in lower_file_name:
        return "FireEye"
    elif "avastnet" in lower_file_name:
        return "AvastNet"

def main(model_directory):
    files = os.listdir(model_directory)
    for file in files:
        if ".pth" in file:
            model_path = os.path.join(model_directory, file)
            model_type = get_model_type(model_path)
            testing_model = test_model(model_path,model_type)
            testing_model.testing()

    
        
    



if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='test model',formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--model_path',default='../models/',type=str,help='model path')
    parser.add_argument('--batch_size',default=32,type=int,help='batch size')
    parser.add_argument('--use_cpu',default=1,type=int,help='number of cpu used')
    parser.add_argument('--test_label_path',default='../data/test_data_label.csv',type=str,help='path of test csv file')
    parser.add_argument('--test_data_path',default='../data/all_files/',type=str,help='path for test files')
    parser.add_argument('--result_path',default='../result/test_results/',type=str,help='parent path for results txt file')

    args = parser.parse_args()
    print('\n',args,'\n')

    if not os.path.exists(args.result_path):
        os.makedirs(args.result_path)

    main(args.model_path)

