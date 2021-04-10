#include <bits/stdc++.h>
#include <chrono> 
using namespace std;
using namespace std::chrono; 

class dataset_point{
    public:
    int id;
    int x_coordinate;
    int y_coordinate;
    dataset_point(){
    }
    dataset_point(int id,int x_coordinate,int y_coordinate){
        this->id=id;
        this->x_coordinate=x_coordinate;
        this->y_coordinate=y_coordinate;
    }
};

class leaf_node{
    public:
    int size;
    vector<dataset_point> points;
    leaf_node(vector<dataset_point> &v){
        size = v.size();
        points = v;
    }
};

class node{
    public:
    int id;
    bool axis;          //true for vertical line(y-axis), false for horizontal line(x-axis)
    int axis_value;
    node* left;
    node* right;
    int which_child;        //0 for root, 1 for left, 2 for right
    node* parent;
    leaf_node* left_leaf;
    leaf_node* right_leaf;

    node(int id,bool axis,int axis_value,int which_child,node* parent_node){
        this->id=id;
        this->axis=axis;
        this->axis_value=axis_value;
        this->left=NULL;
        this->right=NULL;
        this->which_child=which_child;
        this->parent=parent_node;
        this->left_leaf=NULL;
        this->right_leaf=NULL;
    }
};

int total_points,alpha;
int query_point_x,query_point_y;
int k;
int global_xmin,global_xmax,global_ymin,global_ymax;

bool widest_spread_axis(vector<dataset_point> dataset_points){
    int x_min=INT_MAX, x_max=INT_MIN;
    int y_min=INT_MAX, y_max=INT_MIN;
    for(int i=0;i<dataset_points.size();i++){
        if(dataset_points[i].x_coordinate < x_min)
            x_min = dataset_points[i].x_coordinate;
        if(dataset_points[i].x_coordinate > x_max)
            x_max = dataset_points[i].x_coordinate;

        if(dataset_points[i].y_coordinate < y_min)
            y_min = dataset_points[i].y_coordinate;
        if(dataset_points[i].y_coordinate > y_max)
            y_max = dataset_points[i].y_coordinate;
    }
    return (x_max-x_min)>=(y_max-y_min);
}

bool comp_on_x(const dataset_point &d1,const dataset_point &d2){
    return d1.x_coordinate < d2.x_coordinate;
}

bool comp_on_y(const dataset_point &d1,const dataset_point &d2){
    return d1.y_coordinate < d2.y_coordinate;
}

bool comparator(const dataset_point &d1,const dataset_point &d2){
    int dist1 = (d1.x_coordinate-query_point_x)*(d1.x_coordinate-query_point_x) + (d1.y_coordinate-query_point_y)*(d1.y_coordinate-query_point_y);

    int dist2 = (d2.x_coordinate-query_point_x)*(d2.x_coordinate-query_point_x) + (d2.y_coordinate-query_point_y)*(d2.y_coordinate-query_point_y);
    return dist1 < dist2;
}

node* create_kd_tree(int node_id,vector<dataset_point> dataset_points,int which_child,node* parent){
    if(dataset_points.size() <= alpha){
        cout << "This case should not arise\n";
        return NULL;
    }
    bool axis = widest_spread_axis(dataset_points);
    int axis_value;
    vector<dataset_point> temp;
    vector<dataset_point> left_dataset;
    vector<dataset_point> right_dataset;
    temp = dataset_points;
    if(axis){
        sort(temp.begin(),temp.end(),comp_on_x);
        axis_value = temp[(temp.size()-1)/2].x_coordinate;
        for(int i=0;i<dataset_points.size();i++){
            if(dataset_points[i].x_coordinate <= axis_value)
                left_dataset.push_back(dataset_points[i]);
            else
                right_dataset.push_back(dataset_points[i]);
        }
    }
    else{
        sort(temp.begin(),temp.end(),comp_on_y);
        axis_value = temp[(temp.size()-1)/2].y_coordinate;
        for(int i=0;i<dataset_points.size();i++){
            if(dataset_points[i].y_coordinate <= axis_value)
                left_dataset.push_back(dataset_points[i]);
            else
                right_dataset.push_back(dataset_points[i]);            
        }
    }
    node* n = new node(node_id,axis,axis_value,which_child,parent);
    if(right_dataset.size()==0){                //boundary case
        leaf_node* leaf = new leaf_node(left_dataset);
        n->left_leaf = leaf;

        leaf_node* leaf1 = new leaf_node(right_dataset);
        n->right_leaf = leaf1;
        return n;
    }
    if(left_dataset.size() > alpha)
        n->left = create_kd_tree(2*node_id,left_dataset,1,n);
    else{
        leaf_node* leaf = new leaf_node(left_dataset);
        n->left_leaf = leaf;
    }
    if(right_dataset.size() > alpha)
        n->right = create_kd_tree(2*node_id+1,right_dataset,2,n);
    else{
        leaf_node* leaf = new leaf_node(right_dataset);
        n->right_leaf = leaf;
    }
    return n;
}

int height(node* root){
    if(!root)
        return 0;
    return 1 + max(height(root->left),height(root->right));
}

void print_level_wise(node* root,int curr_level,int dest_level)
{
    if(curr_level==dest_level){
        cout << "l" << root->id << " ";
        return;
    }
    if(root->left)
        print_level_wise(root->left,curr_level+1,dest_level);
    else{
        if(curr_level+1==dest_level){
            leaf_node* leaf = root->left_leaf;
            cout << "(";
            for(int i=0;i<(leaf->points).size();i++)
                cout << "(" << (leaf->points)[i].x_coordinate << "," << (leaf->points)[i].y_coordinate << ")";
            cout << ") ";
        }
    }
    if(root->right)
        print_level_wise(root->right,curr_level+1,dest_level);
    else{
        if(curr_level+1==dest_level){
            leaf_node* leaf = root->right_leaf;
            cout << "(";
            for(int i=0;i<(leaf->points).size();i++)
                cout << "(" << (leaf->points)[i].x_coordinate << "," << (leaf->points)[i].y_coordinate << ")";
            cout << ") ";
        }
    }
}

void print_kd_tree(node* root){
    int levels = 1+height(root);
    for(int i=0;i<levels;i++){
        print_level_wise(root,0,i);
        cout << "\n**********************************************************************" << endl;
    }
}

int distance(int x1,int y1,int x2,int y2){
    return ((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2));
}

void insertion_in_pq(vector<dataset_point> &pq,dataset_point d){
    int size = pq.size();
    if(size<k)
        pq.resize(size+1);
    int index=0;
    for(int i=0;i<size;i++){
        dataset_point d1 = pq[i];
        if(distance(query_point_x,query_point_y,d.x_coordinate,d.y_coordinate) < distance(query_point_x,query_point_y,d1.x_coordinate,d1.y_coordinate))
            break;
        index++;
    }
    if(index>=k)
        return;
    for(int i=(pq.size()-1);i>index;i--)
        pq[i]=pq[i-1];
    pq[index]=d;
}

void leaf_processing(leaf_node* leaf,vector<dataset_point> &pq){
    for(int i=0;i<(leaf->points).size();i++)
        insertion_in_pq(pq,(leaf->points)[i]);
}

void set_reigon(node* root,int &x_min,int &x_max,int &y_min,int &y_max){
    if(root->parent==NULL){
        x_min = global_xmin;
        y_min = global_ymin;
        x_max = global_xmax;
        y_max = global_ymax;
        return;
    }
    set_reigon(root->parent,x_min,x_max,y_min,y_max);
    node* parent = root->parent;
    if(root->which_child==1){        //left child
        if(parent->axis)           // vertical line
            x_max = parent->axis_value;
        else                       // horizontal line
            y_max = parent->axis_value;
    }
    else{                           //right child
        if(parent->axis)           //vertical line
            x_min = (parent->axis_value)+1;
        else                      //horizontal line
            y_min = (parent->axis_value)+1;
    }
}

int min_dist_from_reigon(int x_min,int x_max,int y_min,int y_max){
    if( (query_point_x < x_min) && (query_point_y > y_max) )       //1
        return ((x_min-query_point_x)*(x_min-query_point_x)) + ((query_point_y-y_max)*(query_point_y-y_max));

    if( (query_point_x >= x_min) && (query_point_x <= x_max) && (query_point_y >= y_max) )       //2
        return (query_point_y - y_max)*(query_point_y - y_max);

    if( (query_point_x > x_max) && (query_point_y > y_max) )       //3
        return ((query_point_x-x_max)*(query_point_x-x_max)) + ((query_point_y-y_max)*(query_point_y-y_max));

    if( (query_point_x >= x_max) && (query_point_y <= y_max) && (query_point_y >= y_min) )       //4
        return (query_point_x - x_max)*(query_point_x - x_max);

    if( (query_point_x > x_max) && (query_point_y < y_min) )       //5
        return ((query_point_x-x_max)*(query_point_x-x_max)) + ((y_min-query_point_y)*(y_min-query_point_y));

    if( (query_point_x >= x_min) && (query_point_x <= x_max) && (query_point_y <= y_min) )       //6
        return (y_min - query_point_y)*(y_min - query_point_y);

    if( (query_point_x < x_min) && (query_point_y < y_min) )       //7
        return ((x_min-query_point_x)*(x_min-query_point_x)) + ((y_min-query_point_y)*(y_min-query_point_y));

    if( (query_point_x <= x_min) && (query_point_y <= y_max) && (query_point_y >= y_min) )       //8
        return (x_min - query_point_x)*(x_min - query_point_x);
    return 0;
}

void knn_query(node* root,vector<dataset_point> &pq){
    if(pq.size()<k){
        int checker;
        if(root->axis)
            checker = query_point_x;
        else
            checker = query_point_y;
            
        if(checker <= root->axis_value){         //left subtree first
            if(root->left)
                knn_query(root->left,pq);
            else
                leaf_processing(root->left_leaf,pq);
            if(pq.size()<k){
                if(root->right)
                    knn_query(root->right,pq);
                else
                    leaf_processing(root->right_leaf,pq);
            }
            else{
                if(root->right_leaf)
                    leaf_processing(root->right_leaf,pq);
                else{
                    int reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax;
                    set_reigon(root->right,reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
                    int min_dist = min_dist_from_reigon(reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
                    if( distance(pq[k-1].x_coordinate,pq[k-1].y_coordinate,query_point_x,query_point_y) > min_dist)
                        knn_query(root->right,pq);
                }
            }
        }
        else{                   //right subtree first
            if(root->right)
                knn_query(root->right,pq);
            else
                leaf_processing(root->right_leaf,pq);
            if(pq.size()<k){
                if(root->left)
                    knn_query(root->left,pq);
                else
                    leaf_processing(root->left_leaf,pq);
            }
            else{
                if(root->left_leaf)
                    leaf_processing(root->left_leaf,pq);
                else{
                    int reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax;
                    set_reigon(root->left,reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
                    int min_dist = min_dist_from_reigon(reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
                    if( distance(pq[k-1].x_coordinate,pq[k-1].y_coordinate,query_point_x,query_point_y) > min_dist)
                        knn_query(root->left,pq);
                }
            }
        }
    }

    else{
        if(root->left_leaf)
            leaf_processing(root->left_leaf,pq);
        else{
            int reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax;
            set_reigon(root->left,reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
            int min_dist = min_dist_from_reigon(reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
            if( distance(pq[k-1].x_coordinate,pq[k-1].y_coordinate,query_point_x,query_point_y) > min_dist)
                knn_query(root->left,pq);
        }
        if(root->right_leaf)
            leaf_processing(root->right_leaf,pq);
        else{
            int reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax;
            set_reigon(root->right,reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
            int min_dist = min_dist_from_reigon(reigon_xmin,reigon_xmax,reigon_ymin,reigon_ymax);
            if( distance(pq[k-1].x_coordinate,pq[k-1].y_coordinate,query_point_x,query_point_y) > min_dist)
                knn_query(root->right,pq);
        }
    }
}

int main(){
    /* Dataset Creation */
    cout << "Enter the total number of dataset points: ";
    cin >> total_points;
    cout << "Enter the alpha value: ";
    cin >> alpha;
    ofstream file_pointer("dataset_A.txt");
    int id=1,x_coordinate,y_coordinate;
    for(int i=1;i<=total_points;i++){
        x_coordinate = (rand()%401);
        y_coordinate = (rand()%401);
        file_pointer << id << " " << x_coordinate << " " << y_coordinate << endl;
        id++;
    }
    file_pointer.close();

    /* Reading the Dataset */
    ifstream file_pointer1("dataset_A.txt");
    vector<dataset_point> dataset_points;
    global_xmin=INT_MAX;global_ymin=INT_MAX;
    global_xmax=INT_MIN;global_ymax=INT_MIN;
    while(1)
    {
        file_pointer1 >> id;
        if(file_pointer1.eof())
            break;
        file_pointer1 >> x_coordinate >> y_coordinate;
        dataset_point point(id,x_coordinate,y_coordinate);
        dataset_points.push_back(point);
        if(x_coordinate < global_xmin)
            global_xmin = x_coordinate;
        if(x_coordinate > global_xmax)
            global_xmax = x_coordinate;
        if(y_coordinate < global_ymin)
            global_ymin = y_coordinate;
        if(y_coordinate > global_ymax)
            global_ymax = y_coordinate;
    }
    file_pointer1.close();
    
    node* root = create_kd_tree(1,dataset_points,0,NULL);
    cout << "KD tree is created" << endl;
    bool flag=true;
    while(flag){
        cout << "Enter 1 to print the KD tree" << endl;
        cout << "Enter 2 for KNN query" << endl;
        cout << "Enter 3 for naive algorithm" << endl;
        cout << "Enter 4 to exit" << endl;
        int input;
        cin >> input;
        switch(input){
            case 1:{
                print_kd_tree(root);
                break;
            }
            case 2:{
                cout << "Enter the query point: ";
                cin >> query_point_x >> query_point_y;
                cout << "Enter k: ";
                cin >> k;
                vector<dataset_point> pq;
                clock_t start, end; 
                start = clock();
                knn_query(root,pq);
                end = clock();
                double time_taken = double(end - start)*1000 / double(CLOCKS_PER_SEC); 
                cout << "k nearest neighbours: ";
                for(int i=0;i<k;i++){
                    cout << "(" << pq[i].x_coordinate << "," << pq[i].y_coordinate << ") "; 
                }           
                cout << "Time taken: " << fixed << time_taken << setprecision(9); 
                cout << " milliseconds" << endl; 
                break;
            }
            case 3:{
                cout << "Enter the query point: ";
                cin >> query_point_x >> query_point_y;
                cout << "Enter k: ";
                cin >> k;
                vector<dataset_point> temp;
                temp = dataset_points;
                clock_t start, end; 
                start = clock();
                sort(temp.begin(),temp.end(),comparator);
                end = clock();
                double time_taken = double(end - start)*1000 / double(CLOCKS_PER_SEC);   
                cout << "k nearest neighbours: ";
                for(int i=0;i<k;i++){
                    cout << "(" << temp[i].x_coordinate << "," << temp[i].y_coordinate << ") "; 
                }         
                cout << "Time taken: " << fixed << time_taken << setprecision(9); 
                cout << " milliseconds" << endl; 
                break;
            }
            default: {flag=false;}
        }
        cout << "\n";
    }
    return 0;
}