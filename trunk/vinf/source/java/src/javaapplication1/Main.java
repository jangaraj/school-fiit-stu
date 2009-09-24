package javaapplication1;

import java.awt.Window;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.io.Writer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;
import org.openrdf.OpenRDFException;
import org.openrdf.model.Value;
import org.openrdf.query.BindingSet;
import org.openrdf.query.QueryLanguage;
import org.openrdf.query.TupleQuery;
import org.openrdf.query.TupleQueryResult;
import org.openrdf.repository.Repository;
import org.openrdf.repository.RepositoryConnection;
import org.openrdf.repository.RepositoryException;
import org.openrdf.repository.http.HTTPRepository;

/**
 *
 * @author Jae
 */
public class Main {
    private static Writer outt;

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        if(args.length<4) {
          System.out.println("Not enough parameters");
          System.exit(0);
        }
        double x, y, x1, x2, y1, y2;
        x = y = x1 = x2 = y1 = y2 = 0;
        x1 = Double.valueOf(args[0].trim()).doubleValue();
        x2 = Double.valueOf(args[2].trim()).doubleValue();
        y1 = Double.valueOf(args[1].trim()).doubleValue();
        y2 = Double.valueOf(args[3].trim()).doubleValue();

        
       
        
        try {
            String sesameServer = "http://localhost:8080/sesame";
            String repositoryID = "vi";
            Repository myRepository = new HTTPRepository(sesameServer, repositoryID);
            RepositoryConnection con = myRepository.getConnection();
            try {
                String queryString = "" +
                        "PREFIX location:<http://ontology.ui.sav.sk/location.owl#>\n" +
                        "SELECT ?name ?title ?longitude ?latitude\n" +
                        "WHERE {\n" +
                        "?x location:name ?name .\n" +
                        "?x location:hasRegion ?b.\n" +
                        "?b location:title ?title .\n" +
                        "?b location:latitude ?latitude .\n" +
                        "?b location:longitude ?longitude .\n" +
                        "FILTER (?latitude <= " + x1 + ").\n" +
                        "FILTER (?latitude >= " + x2 + ").\n" +
                        "FILTER (?longitude >= " + y1 + ").\n" +
                        "FILTER (?longitude <= " + y2 + ").\n" +
                        "}\n" +
                        "ORDER BY ASC(?name)\n" +
                        "LIMIT 1000\n";
//                System.out.println("QUERY: \n" + queryString);

                TupleQuery tupleQuery = con.prepareTupleQuery(QueryLanguage.SPARQL, queryString);
                System.out.print("<!--\n");
                TupleQueryResult result = tupleQuery.evaluate();
                System.out.print("-->\n");
                
                try {
                    int i=1;
                    int distance_array[];
                    
                    try {
                        outt = new OutputStreamWriter(System.out, "UTF8");
                    } catch (UnsupportedEncodingException ex) {
                        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
                    }
                            
                    while (result.hasNext()) {
                        BindingSet bindingSet = result.next();
                        Value valueOfName = bindingSet.getValue("name");
                        Value valueOfTitle = bindingSet.getValue("title");
                        Value valueOfLongitude = bindingSet.getValue("longitude");
                        Value valueOfLatitude = bindingSet.getValue("latitude");
                        i++;
                        try {
                            outt.write("<b onclick=\"javascript: map.setCenter(new GLatLng(" + valueOfLatitude.stringValue() + ", " + valueOfLongitude.stringValue() + "), 13); return false;\" style=\"cursor:pointer;\">" + valueOfName.stringValue() + "</b> " + valueOfTitle.stringValue() + "<br/>\n");
                        } catch (IOException ex) {
                            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
                        }
                    }
                } finally {
                    result.close();
                }
            } finally {
                con.close();
            }
        } catch (OpenRDFException e) {
            // handle exception
            System.out.println("Error: OpenRDFException");
            System.exit(0);
        }

    }
    
}

