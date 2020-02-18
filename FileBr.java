import java.util.*;
import javax.swing.*; 
import javax.swing.table.*; 
import java.awt.*;
import java.awt.event.*;
import java.sql.*;
public class FileBr extends JFrame implements ActionListener {  
     static String ip,Lo,core;
     String dir;
     static JTextField tf1,tf2;
     static JFrame f,jn;
     static JTable jt;
     static JScrollPane sp;
     static JPanel p;
     static JLabel l1,l2;
     static JButton b,b1;
     static Stack<String> back= new Stack<String>();
     static int x=0;
     static Connection connect=null;
     static final String dbname="filebr";
     static final String url="jdbc:mysql://localhost:3306/" + dbname;
     static final String uname="root";
     static final String pword="root";

     static Statement ps;   
	static{
	System.loadLibrary("clientjni");
	}

      public static JTable retrieveDB(String path){
        String col[]={"Name","Location","Type","Size"};
   			
	String query="Select Name,Location,Type,Size from dir1 where Location='"+path+"'";

        DefaultTableModel tableModel = new DefaultTableModel(col, 0);
		try{
		  ResultSet rs = ps.executeQuery(query);
		  while (rs.next()) {
    		  String name = rs.getString("Name");
    	    	  String loc = rs.getString("Location");
           	  String ty = rs.getString("Type");
		  String si = rs.getString("Size");	 
    		  String[] data = { name, loc , ty ,si} ;		  			  
    	          tableModel.addRow(data);
		   }  }
   		 catch(SQLException re){
			System.out.println("in result"+re);
			}
		JTable jtb=new JTable(tableModel) {
                public boolean editCellAt(int row, int column, java.util.EventObject e) {
            	             return false;                                  //anonymous class used to override editCellAt method
        	           }
      		};
		TableColumn tcol2 = jtb.getColumnModel().getColumn(3);
		jtb.removeColumn(tcol2);
		TableColumn tcol1 = jtb.getColumnModel().getColumn(2);
		jtb.removeColumn(tcol1);
		TableColumn tcol = jtb.getColumnModel().getColumn(1);
		jtb.removeColumn(tcol);
                return jtb;
     }

	public static void createPanel(String path){
        JPanel p2 = new JPanel(new BorderLayout());
	p2.setBounds(x,0,150,600);
        JTable jtemp=retrieveDB(path);

	jtemp.addMouseListener(new Mymouse());

	jtemp.setRowHeight(40);
		/*JScrollPane js = new JScrollPane();
		js.add(jtemp);
		js.getVerticalScrollBar().addAdjustmentListener(new AdjustmentListener(){ 
    
        	public void adjustmentValueChanged(AdjustmentEvent e) {
        	// Check if user has done dragging the scroll bar
        	if(e.getValueIsAdjusting()){
            	JScrollBar scrollBar = (JScrollBar) e.getAdjustable();
            	int extent = scrollBar.getModel().getExtent();
            	int maximum = scrollBar.getModel().getMaximum();
            	if(extent + e.getValue() == maximum){
                JOptionPane.showMessageDialog(jn,"reached end");
            	}
        	}
        	}
        	});*/
	p2.add(new JScrollPane(jtemp));
	if(x>850){
	JOptionPane.showMessageDialog(jn,"reached end");
	}
	//sp.add(p2);
	p.add(p2);
	jn.add(p);
	jn.revalidate();
	x=x+150;
      }
      
       public static void insertDB(String[][] mod)
	{
          
   		String[][] str1=new String[40][100];
   		int i,j;
  		try
  		{
   			for(i=0;i<mod.length;i++)
   			{
    			String str[]=mod[i][0].split("###");
    			for(j=0;j<str.length;j++)
    			{
     			str1[i][j]=String.valueOf(str[j]);
			}
			String Na=String.valueOf(str1[i][0]);
                        Lo=String.valueOf(str1[i][1]);
			String ty=String.valueOf(str1[i][2]);  
			String si=String.valueOf(str1[i][3]);
			  try{
			     ps= connect.createStatement();
			     ps.executeUpdate("INSERT INTO dir1 VALUES ('"+Na+"','"+Lo+"','"+ty+"','"+si+"');");
    			     }
                          catch(SQLException se){
				System.out.println(se);
			  }
   			}
  		}
  		catch(ArrayIndexOutOfBoundsException d){
  		System.out.println("Larger No of directories than u assumed");
  		System.out.println("Increase your array size");
  		}
        }
  
     public static void show(String path)
           {
		JTable jt = retrieveDB(path);
		JPanel p1=new JPanel(new BorderLayout());
		p1.setBounds(0,0,150,900);		//CONSTANT PANEL
		p=new JPanel();
		p.setBounds(150,0,900,900);
             
		p1.add(new JScrollPane(jt));     
		p1.setVisible(true);
		p.setVisible(true);

		jt.addMouseListener(new Mymouse());
   		sp = new JScrollPane();
        	sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		jn.add(p1);
		jn.setSize(1050,1000);
		jt.setRowHeight(40);
		jt.setShowGrid(false);
		p.setLayout(null);
		jn.setLayout(null);
   		jn.setVisible(true);
		jn.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
  	   }
  
  FileBr(){  
  f= new JFrame("Server Connection Form");    
  l1=new JLabel("Enter the IP");  
  l1.setBounds(50,50,100,30);
 
    tf1=new JTextField();
    tf1.setBounds(180,50,100,30);
 
  l2=new JLabel("Enter the Directory.");  
  l2.setBounds(50,100,200,30);

    tf2=new JTextField();
    tf2.setBounds(180,100,100,30);
 
     b=new JButton("ok");
     b.setBounds(165,150,50,30);
     b.addActionListener(this);  
  f.add(l1);
  f.add(tf1);  
  f.add(l2);
  f.add(tf2);
  f.add(b);
  f.setSize(600,600);  
  f.setLayout(null);  
  f.setVisible(true);  
  }

  public static native String[][] filebrowser(String ip,String dir);

  
  public void actionPerformed(ActionEvent e){
   f.setVisible(false);
   jn=new JFrame("Directory Details");
   ip=tf1.getText();
   dir=tf2.getText();
   back.push(dir);
   String[][] s=filebrowser(ip,dir);
   insertDB(s);
   show(dir);  
   }

  static class Mymouse extends MouseAdapter{
     public void mouseClicked(MouseEvent me){
           if (me.getClickCount() == 2) {
	   System.out.println("Double Click");	   
           JTable target=(JTable)me.getSource();
           int row =target.getSelectedRow();
           //int column=target.getSelectedColumn();
           String check=String.valueOf(target.getModel().getValueAt(row,2));
           if(check.equals("Folder"))
              {
	       int column=0;
	       String value=String.valueOf(target.getValueAt(row,column));  
               String value1=String.valueOf(target.getModel().getValueAt(row,column+1));
               String value2=value1+"/";
               String path=value2.concat(value);    
             
               back.push(path);
			String query="select * from dir1 where Location='"+path+"'";
			try{
				ResultSet rs=ps.executeQuery(query);
                        	if(rs.next()==true){
                           	System.out.println("Already in DB");
				//show(path);
				createPanel(path);
				}
				else{
                        	String[][] strr=filebrowser(ip,path);
				System.out.println("Response sending to server");
                        	insertDB(strr);
				//show(path);
				createPanel(path);
 				}
                        }
                        catch(SQLException de){
                         System.out.println("de=="+de);}
              }
           else{
		JOptionPane.showMessageDialog(jn,"Its a file");
             }
        } 
    }
  }
 
    public static void main(String[] a){
    FileBr s= new FileBr();
              try{
		connect = DriverManager.getConnection(url,uname,pword);
		}
                catch(SQLException sse)
                {
                System.out.println(sse);
                }
    }
}