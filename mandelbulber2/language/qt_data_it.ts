<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="it">
<context>
    <name>Form</name>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="14"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="14"/>
        <location filename="../qt_data/primitive_box.ui" line="14"/>
        <location filename="../qt_data/primitive_circle.ui" line="14"/>
        <location filename="../qt_data/primitive_cone.ui" line="14"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="14"/>
        <location filename="../qt_data/primitive_plane.ui" line="14"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="14"/>
        <location filename="../qt_data/primitive_sphere.ui" line="14"/>
        <location filename="../qt_data/primitive_torus.ui" line="14"/>
        <location filename="../qt_data/primitive_water.ui" line="14"/>
        <source>Form</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="35"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="45"/>
        <source> z = SIGN(z) (offset  -  fabs(z)  +  fabs(z) * Slope)   4D</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="195"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="448"/>
        <source>z.y </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="209"/>
        <source>Offset : </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="351"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="135"/>
        <source>z.w</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="358"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="522"/>
        <source>Slope :   </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="393"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="202"/>
        <source>z.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="400"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="385"/>
        <source>DE Tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="407"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="392"/>
        <source>z.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="470"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="103"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="508"/>
        <source>z.w:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="627"/>
        <source>`</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="654"/>
        <source>&lt;p&gt;fabs add conditional&lt;/p&gt;&lt;table&gt;
&lt;/table&gt;
&lt;h3&gt;Code&lt;/h3&gt;
&lt;body style=&quot;background-color:#202020&quot;&gt;
&lt;pre style=&quot;color:#d0d0d0; background-color:#202020;  font-family:&apos;Courier New&apos;,monospace;&quot;&gt;&lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;void&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;TransformFabsAddConditionalIteration&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;CVector3 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;const&lt;/span&gt; cFractal &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; sExtendedAux &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;sign&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) * (&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                        &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset000&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledy&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;sign&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) * (&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                        &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset000&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledz&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;sign&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;) * (&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                        &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset000&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;                                      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.DE = aux.DE * l/L;&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;analyticDE&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// DE tweak&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
&lt;/pre&gt;
&lt;/body&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Fold :</source>
        <translation type="vanished">Folding:</translation>
    </message>
    <message>
        <source>Scale Vary :</source>
        <translation type="vanished">Varia Scala:</translation>
    </message>
    <message>
        <source>&amp;Information</source>
        <translation type="vanished">&amp;Informazioni</translation>
    </message>
    <message>
        <source>c add</source>
        <translation type="vanished">aggiungi c</translation>
    </message>
    <message>
        <source>No parameters</source>
        <translation type="vanished">Non ci sono parametri</translation>
    </message>
    <message>
        <source>Z scale:</source>
        <translation type="vanished">Scala z:</translation>
    </message>
    <message>
        <source>Presets</source>
        <translation type="vanished">Preset</translation>
    </message>
    <message>
        <source>Dodecahedron</source>
        <translation type="vanished">Dodecaedro</translation>
    </message>
    <message>
        <source>Icosahedron</source>
        <translation type="vanished">Icosaedro</translation>
    </message>
    <message>
        <source>Octahedron</source>
        <translation type="vanished">Ottaedro</translation>
    </message>
    <message>
        <source>Menger Sponge</source>
        <translation type="vanished">Spugna di Menger</translation>
    </message>
    <message>
        <source>Main parameters</source>
        <translation type="vanished">Parametri principali</translation>
    </message>
    <message>
        <source>vector offset:</source>
        <translation type="vanished">offset vettori:</translation>
    </message>
    <message>
        <source>alpha:</source>
        <translation type="vanished">alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="275"/>
        <location filename="../qt_data/primitive_box.ui" line="440"/>
        <location filename="../qt_data/primitive_circle.ui" line="159"/>
        <location filename="../qt_data/primitive_cone.ui" line="40"/>
        <location filename="../qt_data/primitive_cone.ui" line="348"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="259"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="424"/>
        <location filename="../qt_data/primitive_plane.ui" line="63"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="230"/>
        <location filename="../qt_data/primitive_sphere.ui" line="201"/>
        <location filename="../qt_data/primitive_sphere.ui" line="280"/>
        <location filename="../qt_data/primitive_torus.ui" line="88"/>
        <location filename="../qt_data/primitive_torus.ui" line="227"/>
        <location filename="../qt_data/primitive_water.ui" line="160"/>
        <source>z:</source>
        <translation></translation>
    </message>
    <message>
        <source>Box Fold offsets</source>
        <translation type="vanished">Offset Folding Box</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="155"/>
        <location filename="../qt_data/primitive_box.ui" line="430"/>
        <location filename="../qt_data/primitive_circle.ui" line="129"/>
        <location filename="../qt_data/primitive_cone.ui" line="338"/>
        <location filename="../qt_data/primitive_cone.ui" line="358"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="189"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="212"/>
        <location filename="../qt_data/primitive_plane.ui" line="183"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="185"/>
        <location filename="../qt_data/primitive_sphere.ui" line="246"/>
        <location filename="../qt_data/primitive_sphere.ui" line="320"/>
        <location filename="../qt_data/primitive_torus.ui" line="193"/>
        <location filename="../qt_data/primitive_torus.ui" line="237"/>
        <location filename="../qt_data/primitive_water.ui" line="352"/>
        <source>y:</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="285"/>
        <location filename="../qt_data/primitive_box.ui" line="410"/>
        <location filename="../qt_data/primitive_circle.ui" line="207"/>
        <location filename="../qt_data/primitive_cone.ui" line="388"/>
        <location filename="../qt_data/primitive_cone.ui" line="424"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="156"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="306"/>
        <location filename="../qt_data/primitive_plane.ui" line="83"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="130"/>
        <location filename="../qt_data/primitive_sphere.ui" line="50"/>
        <location filename="../qt_data/primitive_sphere.ui" line="191"/>
        <location filename="../qt_data/primitive_torus.ui" line="313"/>
        <location filename="../qt_data/primitive_torus.ui" line="333"/>
        <location filename="../qt_data/primitive_water.ui" line="342"/>
        <source>x:</source>
        <translation></translation>
    </message>
    <message>
        <source>Scale:</source>
        <translation type="vanished">Scala:</translation>
    </message>
    <message>
        <source>Menger Sponge mode</source>
        <translation type="vanished">Modo spugna di Menger</translation>
    </message>
    <message>
        <source>alpha</source>
        <translation type="vanished">alfa</translation>
    </message>
    <message>
        <source>&amp;z.z box fold scale,  for normal use make offset z: (above) = 0.00</source>
        <translation type="vanished">&amp;z.z scala box fold, per l&apos;uso normale utilizzare z: (maggiore) = 0.00</translation>
    </message>
    <message>
        <source>z.z scale:</source>
        <translation type="vanished">scala z.z:</translation>
    </message>
    <message>
        <source>Swap z.x z.z</source>
        <translation type="vanished">Scambia z.x z.z</translation>
    </message>
    <message>
        <source>Addition Constant - Offset</source>
        <translation type="vanished">Aggiunta Costante - Offset</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="vanished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <source>alternate swap</source>
        <translation type="vanished">scambio alternato</translation>
    </message>
    <message>
        <source>function order of c.x, c.y and c.z: </source>
        <translation type="vanished">ordine funzione di c.x, c.y e c.z: </translation>
    </message>
    <message>
        <source> &amp;Rotation; </source>
        <translation type="vanished">&amp;Rotazione </translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="vanished">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start Rotation at iteration :</source>
        <translation type="vanished">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Initial Color Min:</source>
        <translation type="vanished">Colore Min iniziale:</translation>
    </message>
    <message>
        <source>Orbit trap component</source>
        <translation type="vanished">Componente orbit trap</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Hint: change coloring algorithm from Standard in the Material Editor&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Consiglio: cambiare algoritmo colore in Standard nell&apos;editor materiali&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Absolute value of z  component  </source>
        <translation type="vanished">Valore assoluto del componente z </translation>
    </message>
    <message>
        <source>Coloring components (beta version only, does not work in hybrid mode)</source>
        <translation type="vanished">Componenti colorazione (solo versione beta, non funziona in modalità ibridi)</translation>
    </message>
    <message>
        <source>edge:</source>
        <translation type="vanished">margine:</translation>
    </message>
    <message>
        <source>Vectors</source>
        <translation type="vanished">Vettori</translation>
    </message>
    <message>
        <source>enable:</source>
        <translation type="vanished">abilita:</translation>
    </message>
    <message>
        <source>direction:</source>
        <translation type="vanished">direzione:</translation>
    </message>
    <message>
        <source>Scale and fold influence:</source>
        <translation type="vanished">Scala e influenza su FoldIng:</translation>
    </message>
    <message>
        <source>Force Cylindrical Fold</source>
        <translation type="vanished">Forza Folding cilindrico</translation>
    </message>
    <message>
        <source>addCpi&amp;xel  ConstantMultiplier c.x c.y swap</source>
        <translation type="vanished">addCpi&amp;xel  ConstantMultiplier scambio c.x c.y </translation>
    </message>
    <message>
        <source>Rotation</source>
        <translation type="vanished">Rotazione</translation>
    </message>
    <message>
        <source>Info&amp;rmation</source>
        <translation type="vanished">Info&amp;rmazione</translation>
    </message>
    <message>
        <source>Start at iteration :</source>
        <translation type="vanished">Inizia all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Stop at Iteration:</source>
        <translation type="vanished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Box folding value:</source>
        <translation type="vanished">Valore di Box folding:</translation>
    </message>
    <message>
        <source>Fold Type 2</source>
        <translation type="vanished">Folding tipo 2</translation>
    </message>
    <message>
        <source>Fold Type 3</source>
        <translation type="vanished">Folding tipo 3</translation>
    </message>
    <message>
        <source>Fold Type 4</source>
        <translation type="vanished">Folding tipo 4</translation>
    </message>
    <message>
        <source>Fold Types</source>
        <translation type="vanished">Tipi Folding</translation>
    </message>
    <message>
        <source>Fold Type 1</source>
        <translation type="vanished">Folding tipo 1</translation>
    </message>
    <message>
        <source>Fold Parameters</source>
        <translation type="vanished">Parametri di Folding</translation>
    </message>
    <message>
        <source>Fold Type 5</source>
        <translation type="vanished">Folding tipo 5</translation>
    </message>
    <message>
        <source>type 1</source>
        <translation type="vanished">tipo 1</translation>
    </message>
    <message>
        <source>type 2</source>
        <translation type="vanished">tipo 2</translation>
    </message>
    <message>
        <source>type 3</source>
        <translation type="vanished">tipo 3</translation>
    </message>
    <message>
        <source>type 4</source>
        <translation type="vanished">tipo 4</translation>
    </message>
    <message>
        <source>type 5</source>
        <translation type="vanished">tipo 5</translation>
    </message>
    <message>
        <source>order of Fold Types</source>
        <translation type="vanished">ordine tipi di Folding</translation>
    </message>
    <message>
        <source>Stop at iteration:</source>
        <translation type="vanished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Enable slot sequence:  </source>
        <translation type="vanished">Abilita sequenza slot:</translation>
    </message>
    <message>
        <source>Slot , Fold Type and  Iteration controls</source>
        <translation type="vanished">Slot, Tipo Folding e controlli Iterazioni</translation>
    </message>
    <message>
        <source>z.z Fold Value</source>
        <translation type="vanished">Valore di folding z.z</translation>
    </message>
    <message>
        <source>enable z.z  scale</source>
        <translation type="vanished">abilita scala z.z</translation>
    </message>
    <message>
        <source>z.z Fold Limit</source>
        <translation type="vanished">Limite folding z.z</translation>
    </message>
    <message>
        <source>Standard &amp;Functions Enabled</source>
        <translation type="vanished">&amp;Funzioni standard abilitate</translation>
    </message>
    <message>
        <source>Start Fold at iteration :</source>
        <translation type="vanished">Inizia Folding all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Stop Fold at iteration:</source>
        <translation type="vanished">Ferma Folding all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Abox spherical folding :</source>
        <translation type="vanished">Folding sferico Abox: </translation>
    </message>
    <message>
        <source>Mbox spherical folding :</source>
        <translation type="vanished">Folding sferico Mbox: </translation>
    </message>
    <message>
        <source>Stop Scale at iteration:</source>
        <translation type="vanished">Ferma scala all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start Scale at iteration :</source>
        <translation type="vanished">Inizia Scala all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Scale : </source>
        <translation type="vanished">Scala:</translation>
    </message>
    <message>
        <source>&amp;Main rotation</source>
        <translation type="vanished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <source>Fixed Radius:</source>
        <translation type="vanished">Raggio fisso:</translation>
    </message>
    <message>
        <source>Enable   internal addCpixel  constant / Julia constant swap</source>
        <translation type="vanished">Abilita scambio interno tra le costanti addCpixel/Julia </translation>
    </message>
    <message>
        <source>Single Formula addCpixel  Constant Multiplier</source>
        <translation type="vanished">Formula singola - Moltiplicatore costante addCpixel</translation>
    </message>
    <message>
        <source>Single Formula Julia mode</source>
        <translation type="vanished">Formula singola - modo Julia</translation>
    </message>
    <message>
        <source>Julia constant </source>
        <translation type="vanished">Costante di Julia</translation>
    </message>
    <message>
        <source>Minimum Radius:</source>
        <translation type="vanished">Raggio minimo:</translation>
    </message>
    <message>
        <source>Start at iteration:</source>
        <translation type="vanished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <source>&amp;Box Fold  </source>
        <translation type="vanished">Folding &amp;Box  </translation>
    </message>
    <message>
        <source>Spherical Fold; </source>
        <translation type="vanished">Folding Sferico: </translation>
    </message>
    <message>
        <source>Scale</source>
        <translation type="vanished">Scala</translation>
    </message>
    <message>
        <source>&amp;Rotation  </source>
        <translation type="vanished">&amp;Rotazione </translation>
    </message>
    <message>
        <source>Infor&amp;mation</source>
        <translation type="vanished">Infor&amp;mazione</translation>
    </message>
    <message>
        <source>Offset Radius:</source>
        <translation type="vanished">Raggio Offset:</translation>
    </message>
    <message>
        <source>Offset. Use in normal mode or use it to fine tune in Julia mode</source>
        <translation type="vanished">Offset. Da usare normalmente o per mettere a punto le configurazioni dei parametri relativi alla modalità Julia</translation>
    </message>
    <message>
        <source>A&amp;nalytic Logrithmic DE adjustment</source>
        <translatorcomment>&quot;Logrithmic&quot; missing an &quot;a&quot;</translatorcomment>
        <translation type="vanished">Modifica DE A&amp;naltico logaritmica</translation>
    </message>
    <message>
        <source>Analytic Logrithmic DE multiplier: </source>
        <translation type="vanished">Moltiplicatore DE analtico logaritmico:</translation>
    </message>
    <message>
        <source>addCpixel Symmetrical  Constant Multiplier </source>
        <translation type="vanished">addCpixel -Moltiplicatore simmetrico costanti </translation>
    </message>
    <message>
        <source>zA after itr.:</source>
        <translation type="vanished">zA dopo itr.:</translation>
    </message>
    <message>
        <source>Constant Z:</source>
        <translation type="vanished">Constante Z:</translation>
    </message>
    <message>
        <source>Constant A:</source>
        <translation type="vanished">Constante A:</translation>
    </message>
    <message>
        <source> Multiplier  </source>
        <translation type="vanished">Moltiplicatore</translation>
    </message>
    <message>
        <source>&amp;Box Offset</source>
        <translation type="vanished">Offset &amp;Box</translation>
    </message>
    <message>
        <source>Stop  Offset at iteration:</source>
        <translation type="vanished">Ferma Offset all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Start Offset at iteration :</source>
        <translation type="vanished">Inizia Offset all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Box Offset:  </source>
        <translation type="vanished">Offset Box:  </translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Can sometimes improve render quality when using analytic DE&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A volte può migliorare la qualità di renering usando la DE analitica&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Analytic DE calculation</source>
        <translation type="vanished">Calcolo DE analitica</translation>
    </message>
    <message>
        <source> I&amp;teration Weight.;  z  =  (  z * const.Z) + (  zA * const.A) + ( zB * const.B)</source>
        <translation type="vanished">Peso  I&amp;terazione z  =  (z * const.Z) + (zA * const.A) + (zB * const.B)</translation>
    </message>
    <message>
        <source>zB after itr.:</source>
        <translation type="vanished">zB dopo itr.:</translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="vanished">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <source>Constant B:</source>
        <translation type="vanished">Costante B:</translation>
    </message>
    <message>
        <source>Spherical folding
fixed radius:</source>
        <translation type="vanished">Raggio fisso
Folding sferico: </translation>
    </message>
    <message>
        <source>Box folding limit:</source>
        <translation type="vanished">Limite box Folding:</translation>
    </message>
    <message>
        <source>Spherical folding
min radius:</source>
        <translation type="vanished">Raggio minimo 
Folding sferico:</translation>
    </message>
    <message>
        <source>Spherical folding
offset:</source>
        <translation type="vanished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <source>&amp;Rotation of separate planes</source>
        <translation type="vanished">&amp;Rotazione piani separati</translation>
    </message>
    <message>
        <source>X Axis</source>
        <translation type="vanished">Asse X</translation>
    </message>
    <message>
        <source>Positive plane</source>
        <translation type="vanished">Piano positivo</translation>
    </message>
    <message>
        <source>Negative plane</source>
        <translation type="vanished">Piano negativo</translation>
    </message>
    <message>
        <source>Y Axis</source>
        <translation type="vanished">Asse Y</translation>
    </message>
    <message>
        <source>Z Axis</source>
        <translation type="vanished">Asse Z</translation>
    </message>
    <message>
        <source>Solid:</source>
        <translation type="vanished">Solido:</translation>
    </message>
    <message>
        <source>Box type:</source>
        <translation type="vanished">Tipo Box:</translation>
    </message>
    <message>
        <source>Melt:</source>
        <translation type="vanished">Sciogli:</translation>
    </message>
    <message>
        <source>Coloring parameters</source>
        <translation type="vanished">Parametri colorazione</translation>
    </message>
    <message>
        <source>Min radius component:</source>
        <translation type="vanished">Componente raggio Min:</translation>
    </message>
    <message>
        <source>Fold component</source>
        <translation type="vanished">Componente Folding</translation>
    </message>
    <message>
        <source>Fold function components -group weight:   </source>
        <translation type="vanished">Componenti funzione Fold -peso gruppo:</translation>
    </message>
    <message>
        <source>Scale component  </source>
        <translation type="vanished">Componente Scala</translation>
    </message>
    <message>
        <source>Y plane component:</source>
        <translation type="vanished">Componente piano Y:</translation>
    </message>
    <message>
        <source>X plane component:</source>
        <translation type="vanished">Componente piano Y:</translation>
    </message>
    <message>
        <source>Power Variable:</source>
        <translation type="vanished">Variabile Potenza:</translation>
    </message>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="vanished">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Start varying at iteration :</source>
        <translation type="vanished">Inizia variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Enable sine mode (disable cosine mode)</source>
        <translation type="vanished">Abilita modo seno (disabilita modo coseno)</translation>
    </message>
    <message>
        <source>Polar to Cartesian -  Cosine Mode z.z = cos(th);</source>
        <translation type="vanished">Da Polare a Cartesiano - Modo coseno z.z = cos(th);</translation>
    </message>
    <message>
        <source>Rotate about z axis:  </source>
        <translation type="vanished">Ruota attorno all&apos;asse z: </translation>
    </message>
    <message>
        <source>Enable seperate &amp;trig control of second  function</source>
        <translation type="vanished">Sbilita controllo &amp;trig separato della seconda funzione</translation>
    </message>
    <message>
        <source>ph0 function2: </source>
        <translation type="vanished">funzione2 ph0: </translation>
    </message>
    <message>
        <source>th0 function2: </source>
        <translation type="vanished">funzione2 th0: </translation>
    </message>
    <message>
        <source>Phi  Scale:</source>
        <translation type="vanished">Scala Phi: </translation>
    </message>
    <message>
        <source>ph0 function: </source>
        <translation type="vanished">Funzione ph0: </translation>
    </message>
    <message>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier </source>
        <translation type="vanished">addCpi&amp;xel Scambia Assi ConstantMultiplier </translation>
    </message>
    <message>
        <source>function order of x, y and z: </source>
        <translation type="vanished">ordine funzione x,y e z: </translation>
    </message>
    <message>
        <source>th0 function: </source>
        <translation type="vanished">funzione th0: </translation>
    </message>
    <message>
        <source>Z plane component:</source>
        <translation type="vanished">componente piano Z:</translation>
    </message>
    <message>
        <source>Absolute value of z:</source>
        <translation type="vanished">Valore assoluto di z:</translation>
    </message>
    <message>
        <source>Fixed radius component:</source>
        <translation type="vanished">Componente raggio fisso:</translation>
    </message>
    <message>
        <source>Tetrahedron</source>
        <translation type="vanished">Tetraedro</translation>
    </message>
    <message>
        <source>Cube</source>
        <translation type="vanished">Cubo</translation>
    </message>
    <message>
        <source>Spherical fold size:</source>
        <translation type="vanished">Dimensioni Folding sferico:</translation>
    </message>
    <message>
        <source>R power:</source>
        <translation type="vanished">Potenza R:</translation>
    </message>
    <message>
        <source>Box fold size:</source>
        <translation type="vanished">Dimensioni Folding Box:</translation>
    </message>
    <message>
        <source>Vary scale:</source>
        <translation type="vanished">Varia Scala:</translation>
    </message>
    <message>
        <source>w-axis constant:</source>
        <translation type="vanished">Costante asse -w:</translation>
    </message>
    <message>
        <source>Alpha angle offset:</source>
        <translation type="vanished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <source>Power:</source>
        <translation type="vanished">Potenza:</translation>
    </message>
    <message>
        <source>zAngle Power:</source>
        <translation type="vanished">Angolo z Potenza:</translation>
    </message>
    <message>
        <source>Beta angle offset:</source>
        <translation type="vanished">Offset angolo Beta:</translation>
    </message>
    <message>
        <source>Gamma angle offset:</source>
        <translation type="vanished">Offset angolo Gamma:</translation>
    </message>
    <message>
        <source>Sharpness:</source>
        <translation type="vanished">Dettaglio angoli:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="53"/>
        <location filename="../qt_data/primitive_circle.ui" line="105"/>
        <location filename="../qt_data/primitive_cone.ui" line="218"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="232"/>
        <location filename="../qt_data/primitive_plane.ui" line="255"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="65"/>
        <location filename="../qt_data/primitive_sphere.ui" line="164"/>
        <location filename="../qt_data/primitive_torus.ui" line="98"/>
        <location filename="../qt_data/primitive_water.ui" line="227"/>
        <source>Y-axis rotation:</source>
        <translation>Rotazione asse Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="98"/>
        <location filename="../qt_data/primitive_circle.ui" line="122"/>
        <location filename="../qt_data/primitive_cone.ui" line="50"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="142"/>
        <location filename="../qt_data/primitive_plane.ui" line="193"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="178"/>
        <location filename="../qt_data/primitive_sphere.ui" line="256"/>
        <location filename="../qt_data/primitive_torus.ui" line="186"/>
        <location filename="../qt_data/primitive_water.ui" line="91"/>
        <source>X-axis rotation:</source>
        <translation>Rotazione asse X:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="135"/>
        <location filename="../qt_data/primitive_circle.ui" line="40"/>
        <location filename="../qt_data/primitive_cone.ui" line="188"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="404"/>
        <location filename="../qt_data/primitive_plane.ui" line="169"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="321"/>
        <location filename="../qt_data/primitive_sphere.ui" line="273"/>
        <location filename="../qt_data/primitive_torus.ui" line="401"/>
        <location filename="../qt_data/primitive_water.ui" line="440"/>
        <source>Position:</source>
        <translation>Posizione:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="305"/>
        <source>rounding:</source>
        <translation>arrotondamento:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="315"/>
        <source>Size:</source>
        <translation>Dimensioni:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="350"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="288"/>
        <source>depth (y):</source>
        <translation>profondità (y):</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="360"/>
        <location filename="../qt_data/primitive_cone.ui" line="133"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="286"/>
        <source>height (z):</source>
        <translation>altezza (y):</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="370"/>
        <location filename="../qt_data/primitive_circle.ui" line="139"/>
        <location filename="../qt_data/primitive_cone.ui" line="181"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="149"/>
        <location filename="../qt_data/primitive_plane.ui" line="176"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="113"/>
        <location filename="../qt_data/primitive_sphere.ui" line="303"/>
        <location filename="../qt_data/primitive_torus.ui" line="118"/>
        <location filename="../qt_data/primitive_water.ui" line="335"/>
        <source>Z-axis rotation:</source>
        <translation>Rotazione asse Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="206"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="348"/>
        <source>width (x):</source>
        <translation>Larghezza (x):</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="241"/>
        <location filename="../qt_data/primitive_circle.ui" line="296"/>
        <location filename="../qt_data/primitive_cone.ui" line="434"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="434"/>
        <location filename="../qt_data/primitive_plane.ui" line="248"/>
        <location filename="../qt_data/primitive_rectangle.ui" line="195"/>
        <location filename="../qt_data/primitive_sphere.ui" line="343"/>
        <location filename="../qt_data/primitive_torus.ui" line="415"/>
        <location filename="../qt_data/primitive_water.ui" line="447"/>
        <source>Material:</source>
        <translation>Materiale:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="450"/>
        <location filename="../qt_data/primitive_cone.ui" line="417"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="182"/>
        <location filename="../qt_data/primitive_plane.ui" line="200"/>
        <location filename="../qt_data/primitive_sphere.ui" line="336"/>
        <location filename="../qt_data/primitive_torus.ui" line="111"/>
        <location filename="../qt_data/primitive_water.ui" line="433"/>
        <source>Empty inside</source>
        <translation>Vuoto all&apos;interno</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_box.ui" line="248"/>
        <location filename="../qt_data/primitive_cone.ui" line="235"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="279"/>
        <location filename="../qt_data/primitive_sphere.ui" line="373"/>
        <location filename="../qt_data/primitive_torus.ui" line="408"/>
        <source>Repeat</source>
        <translation>Ripetere</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_circle.ui" line="47"/>
        <location filename="../qt_data/primitive_cone.ui" line="265"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="249"/>
        <source>radius (x-y plane):</source>
        <translation>raggio (piano x-y):</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_cone.ui" line="404"/>
        <location filename="../qt_data/primitive_cylinder.ui" line="135"/>
        <source>Caps</source>
        <translation>Tappi</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_sphere.ui" line="211"/>
        <source>Radius:</source>
        <translation>Raggio:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_torus.ui" line="203"/>
        <source>Torus radius:</source>
        <translation>Raggio Toro:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_torus.ui" line="220"/>
        <source>Tube radius:</source>
        <translation>Raggio tubo:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_water.ui" line="140"/>
        <source>Wave speed (anim.):</source>
        <translation>Velocità onda (anim.):</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_water.ui" line="392"/>
        <source>Wave amplitude:</source>
        <translation>Ampiezza onda:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_water.ui" line="105"/>
        <source>Wave length:</source>
        <translation>Lunghezza onda:</translation>
    </message>
    <message>
        <location filename="../qt_data/primitive_water.ui" line="98"/>
        <source>Iterations:</source>
        <translation>Iterazioni:</translation>
    </message>
    <message>
        <source>Positive Z component</source>
        <translation type="vanished">Componente Z positivo</translation>
    </message>
    <message>
        <source>Y scale:</source>
        <translation type="vanished">Scala y:</translation>
    </message>
    <message>
        <source>Quaternion Scales   </source>
        <translation type="vanished">Scale Quaternione </translation>
    </message>
    <message>
        <source>Addition Constant  </source>
        <translation type="vanished">Aggiunta Costante  </translation>
    </message>
    <message>
        <source>Frequency:</source>
        <translation type="vanished">Frequenza:</translation>
    </message>
    <message>
        <source>Amplitude:</source>
        <translation type="vanished">Ampiezza:</translation>
    </message>
    <message>
        <source>Fold x:</source>
        <translation type="vanished">Fold x:</translation>
    </message>
    <message>
        <source>Cylinder Half Size</source>
        <translation type="vanished">Dimensioni Metà Cilindro </translation>
    </message>
    <message>
        <source>Fold z:</source>
        <translation type="vanished">Fold z:</translation>
    </message>
    <message>
        <source>Fold y:</source>
        <translation type="vanished">Fold y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="455"/>
        <source>two limit mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="462"/>
        <source>one limit mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="476"/>
        <source>LimitB : </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="579"/>
        <source> z = SIGN(z) ((1.0/LimitA )   -    (1.0/(fabs(z) +LimitA))    +  (fabs(z) * Slope))</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="674"/>
        <source>LimitA :   </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="709"/>
        <source> z = SIGN(z) (LimitB   -   (1.0/(fabs(z) +LimitA))    +   (fabs(z) * Slope))</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="909"/>
        <source>I&amp;nformation</source>
        <translation>I&amp;nformazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="936"/>
        <source>&lt;p&gt;Reciprocal3 from M3D, Darkbeam&apos;s code&lt;/p&gt;&lt;table&gt;
&lt;/table&gt;
&lt;h3&gt;Code&lt;/h3&gt;
&lt;body style=&quot;background-color:#202020&quot;&gt;
&lt;pre style=&quot;color:#d0d0d0; background-color:#202020;  font-family:&apos;Courier New&apos;,monospace;&quot;&gt;&lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;void&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;TransformReciprocal3Iteration&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;CVector3 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;const&lt;/span&gt; cFractal &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; sExtendedAux &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
  CVector3 tempZ &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
      tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;= (&lt;/span&gt;&lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/ (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAxFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
      tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;= (&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offsetA111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/ (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

    tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset000&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// function slope&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;sign&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledy&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
      tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;= (&lt;/span&gt;&lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/ (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAxFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
      tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;= (&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offsetA111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/ (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

    tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset000&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;sign&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledz&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
      tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;= (&lt;/span&gt;&lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/ (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAxFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
      tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;= (&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offsetA111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/ (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

    tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset000&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;sign&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;) *&lt;/span&gt; tempZ&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.DE = aux.DE * l/L;&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;analyticDE&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// DE tweak&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
&lt;/pre&gt;
&lt;/body&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="vanished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <source>Internal &amp;Rotation; </source>
        <translatorcomment>&quot;;&quot; should be removed, maybe?</translatorcomment>
        <translation type="vanished">&amp;Rotazione Interna </translation>
    </message>
    <message>
        <source>Enable fabs(z. x)</source>
        <translation type="vanished">Abilita fabs(z.x)</translation>
    </message>
    <message>
        <source>Enable fabs( z.z)</source>
        <translation type="vanished">Abilita fabs( z.z)</translation>
    </message>
    <message>
        <source>Enable fabs (z.y)</source>
        <translation type="vanished">Abilita fabs( z.y)</translation>
    </message>
    <message>
        <source>XY multiplier:</source>
        <translation type="vanished">moltiplicatore XY:</translation>
    </message>
    <message>
        <source>Sqr Multiplier : </source>
        <translation type="vanished">Moltiplicatore Sqr: </translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>doubts over &quot;signed&quot;</translatorcomment>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>addCpi&amp;xel Symmetrical  Constant Multiplier </source>
        <translation type="vanished">addCpi&amp;xel Moltiplicatore Costante Simmetrico </translation>
    </message>
    <message>
        <source> Cpixel Multiplier </source>
        <translation type="vanished">Moltiplicatore Cpixel </translation>
    </message>
    <message>
        <source>Type 1 </source>
        <translation type="vanished">Tipo 1 </translation>
    </message>
    <message>
        <source>if ( z &gt; Fold) z =  Value -z,   else if( z &lt;Fold) z = - Value - z, </source>
        <translation type="vanished">if ( z &gt; Fold) z =  Value -z,   else if( z &lt;Fold) z = - Value - z, </translation>
    </message>
    <message>
        <source>Type 2</source>
        <translation type="vanished">Tipo 2</translation>
    </message>
    <message>
        <source>Type 4 Fold Value</source>
        <translation type="vanished">Tipo 4 Valore di Fold</translation>
    </message>
    <message>
        <source>Fold Type</source>
        <translation type="vanished">Tipo Fold</translation>
    </message>
    <message>
        <source>Type 5 Fold2 Value</source>
        <translation type="vanished">Tipo 5 Valore di Fold</translation>
    </message>
    <message>
        <source>Type 3 </source>
        <translation type="vanished">Tpo 3</translation>
    </message>
    <message>
        <source>Type 5</source>
        <translation type="vanished">Tipo 5</translation>
    </message>
    <message>
        <source>Type 4</source>
        <translation type="vanished">Tipo 4</translation>
    </message>
    <message>
        <source>Parameters</source>
        <translation type="vanished">Parametri</translation>
    </message>
    <message>
        <source>Sqrt Minimum Radius :</source>
        <translation type="vanished">Minimo Raggio Sqrt: </translation>
    </message>
    <message>
        <source>&lt;p&gt;Based on Amazing Surf Mod 1 from Mandelbulber3D, a formula proposed by Kali,&lt;br&gt;with features added by Darkbeam&lt;/p&gt;&lt;table&gt;
&lt;tr&gt;&lt;th&gt;Reference&lt;/th&gt;&lt;td&gt;&lt;a href=&quot;????&quot;&gt;????&lt;br&gt;
&lt;/td&gt;&lt;/tr&gt;
&lt;/table&gt;
&lt;h3&gt;Code&lt;/h3&gt;
&lt;body style=&quot;background-color:#202020&quot;&gt;
&lt;pre style=&quot;color:#d0d0d0; background-color:#202020;  font-family:&apos;Courier New&apos;,monospace;&quot;&gt;&lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;void&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;AmazingSurfMod1Iteration&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;CVector3 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;const&lt;/span&gt; cFractal &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; sExtendedAux &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelboxVary4D&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scaleVary &lt;span style=&quot;color:#d0d0d0&quot;&gt;* (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// z = fold - fabs( fabs(z) - fold)&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAyFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAzFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// if z &amp;gt; limit) z =  Value -z,   else if z &amp;lt; limit) z = - Value - z,&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledxFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;gt;&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.x;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;else if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt; -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;= -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.x;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;gt;&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.y;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;else if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt; -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;= -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.y;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledyFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                 &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                 &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  z &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant000&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;

  &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;double&lt;/span&gt; r2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;Dot&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// force cylinder fold&lt;/span&gt;
    r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;-=&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// if (r2 &amp;lt; 1e-21)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0a0&quot;&gt;//  r2 = 1e-21;&lt;/span&gt;

  &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;double&lt;/span&gt; sqrtMinR &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;sqtR&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt;&lt;/span&gt; sqrtMinR&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;mboxFactor1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;mboxFactor1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factorSp1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;else if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt;&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; r2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; r2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factorSp2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale1
       &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;* (&lt;/span&gt;&lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale1&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  z &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;rotationMatrix&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;RotateVector&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldFactor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;compFold0&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// fold group weight&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;minRFactor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;compMinR&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// orbit trap weight&lt;/span&gt;

  &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;double&lt;/span&gt; scaleColor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;colorMin &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// scaleColor += fabs(fractal-&amp;gt;mandelbox.scale);&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scaleFactor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; scaleColor &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;compScale&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
&lt;/pre&gt;
&lt;/body&gt;</source>
        <translatorcomment>Change Mandelbulber3D</translatorcomment>
        <translation type="vanished">&lt;p&gt;Formula basata sulla formula Amazing Surf Mod 1 proveniente da Mandelbulb3D, formula proposta da Kali,&lt;br&gt;con funzionalità aggiunte da Darkbeam&lt;/p&gt;&lt;table&gt;
&lt;tr&gt;&lt;th&gt;Reference&lt;/th&gt;&lt;td&gt;&lt;a href=&quot;????&quot;&gt;????&lt;br&gt;
&lt;/td&gt;&lt;/tr&gt;
&lt;/table&gt;
&lt;h3&gt;Code&lt;/h3&gt;
&lt;body style=&quot;background-color:#202020&quot;&gt;
&lt;pre style=&quot;color:#d0d0d0; background-color:#202020;  font-family:&apos;Courier New&apos;,monospace;&quot;&gt;&lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;void&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;AmazingSurfMod1Iteration&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;CVector3 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;const&lt;/span&gt; cFractal &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;,&lt;/span&gt; sExtendedAux &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;amp;&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelboxVary4D&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scaleVary &lt;span style=&quot;color:#d0d0d0&quot;&gt;* (&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAx&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// z = fold - fabs( fabs(z) - fold)&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAyFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;) -&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledAzFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// if z &amp;gt; limit) z =  Value -z,   else if z &amp;lt; limit) z = - Value - z,&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledxFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;gt;&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.x;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;else if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt; -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;= -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.x;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;gt;&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.y;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;else if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt; -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
      z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;= -&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldingValue &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
      &lt;span style=&quot;color:#d0d0a0&quot;&gt;// aux.color += fractal-&amp;gt;mandelbox.color.factor.y;&lt;/span&gt;
    &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledyFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                 &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
                 &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;offset2&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
          &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant111&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;x&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;y&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  z &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;additionConstant000&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;

  &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;double&lt;/span&gt; r2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;Dot&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;functionEnabledFalse&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// force cylinder fold&lt;/span&gt;
    r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;-=&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; z&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;

  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// if (r2 &amp;lt; 1e-21)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0a0&quot;&gt;//  r2 = 1e-21;&lt;/span&gt;

  &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;double&lt;/span&gt; sqrtMinR &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;sqtR&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt;&lt;/span&gt; sqrtMinR&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;mboxFactor1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;mboxFactor1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factorSp1&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
  &lt;span style=&quot;color:#7ec0ee; font-weight:bold&quot;&gt;else if&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;r2 &lt;span style=&quot;color:#d0d0d0&quot;&gt;&amp;lt;&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;)&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;{&lt;/span&gt;
    z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; r2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;/&lt;/span&gt; r2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
    aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color &lt;span style=&quot;color:#d0d0d0&quot;&gt;+=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;color&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;factorSp2&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;

  z &lt;span style=&quot;color:#d0d0d0&quot;&gt;*=&lt;/span&gt; aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale1
       &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;* (&lt;/span&gt;&lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt; &lt;span style=&quot;color:#d0d0d0&quot;&gt;-&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale1&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;DE &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;actualScale&lt;span style=&quot;color:#d0d0d0&quot;&gt;) +&lt;/span&gt; &lt;span style=&quot;color:#87df71&quot;&gt;1.0&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
  z &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;transformCommon&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;rotationMatrix&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;&lt;span style=&quot;color:#e47eed&quot;&gt;RotateVector&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;z&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;

  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;foldFactor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;compFold0&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt; &lt;span style=&quot;color:#d0d0a0&quot;&gt;// fold group weight&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;minRFactor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;compMinR&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// orbit trap weight&lt;/span&gt;

  &lt;span style=&quot;color:#f09479; font-weight:bold&quot;&gt;double&lt;/span&gt; scaleColor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;colorMin &lt;span style=&quot;color:#d0d0d0&quot;&gt;+&lt;/span&gt; &lt;span style=&quot;color:#e47eed&quot;&gt;fabs&lt;/span&gt;&lt;span style=&quot;color:#d0d0d0&quot;&gt;(&lt;/span&gt;fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;mandelbox&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scale&lt;span style=&quot;color:#d0d0d0&quot;&gt;);&lt;/span&gt;
  &lt;span style=&quot;color:#d0d0a0&quot;&gt;// scaleColor += fabs(fractal-&amp;gt;mandelbox.scale);&lt;/span&gt;
  aux&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;scaleFactor &lt;span style=&quot;color:#d0d0d0&quot;&gt;=&lt;/span&gt; scaleColor &lt;span style=&quot;color:#d0d0d0&quot;&gt;*&lt;/span&gt; fractal&lt;span style=&quot;color:#d0d0d0&quot;&gt;-&amp;gt;&lt;/span&gt;foldColor&lt;span style=&quot;color:#d0d0d0&quot;&gt;.&lt;/span&gt;compScale&lt;span style=&quot;color:#d0d0d0&quot;&gt;;&lt;/span&gt;
&lt;span style=&quot;color:#d0d0d0&quot;&gt;}&lt;/span&gt;
&lt;/pre&gt;
&lt;/body&gt;</translation>
    </message>
    <message>
        <source>This formula does not use the global addCpixel constant multiplier (on Options tab)</source>
        <translation type="vanished">Questa formula non usa il moltiplicatore di costante a livello globale (su scheda Opzioni)</translation>
    </message>
    <message>
        <source>Mag Transform T&amp;1</source>
        <translation type="vanished">Trasformazione Mag  T&amp;1</translation>
    </message>
    <message>
        <source>Stop T1 at iteration:</source>
        <translation type="vanished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Start T1 at iteration :</source>
        <translation type="vanished">Inizia T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Scale </source>
        <translation type="vanished">Scala</translation>
    </message>
    <message>
        <source>Mag Transform &amp;T1   Internal Rotation; </source>
        <translation type="vanished">Trasformazione Mag &amp;T1 Rotazione Interna </translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;In this formula the c.y and c.z components of &amp;quot;CVector3 c&amp;quot; are swapped before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Pine T&amp;ree  addCpixel ConstantMultiplier  ( c.y c.z Swap)</source>
        <translation type="vanished">Pino Moltiplicatore Costante addCpixel (Scambia c.y c.z)</translation>
    </message>
    <message>
        <source>Stop PT at iteration:</source>
        <translation type="vanished">Ferma PT all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start PT at iteration :</source>
        <translation type="vanished">Inizia PT all&apos;iterazione:</translation>
    </message>
    <message>
        <source>alternate c.y c.z swap</source>
        <translation type="vanished">Alterna scambio c.y c.z</translation>
    </message>
    <message>
        <source>Cpixel Multiplier  </source>
        <translation type="vanished">Moltiplicatore Cpixel  </translation>
    </message>
    <message>
        <source>z.y Multiplier::</source>
        <translation type="vanished">Moltiplicatore z.y  </translation>
    </message>
    <message>
        <source>z scale:</source>
        <translation type="vanished">scala z:</translation>
    </message>
    <message>
        <source>Mag Transform &amp;T1   Rotation</source>
        <translation type="vanished">Trasformazione Mag Rotazione &amp;T1</translation>
    </message>
    <message>
        <source>Try both Linear and Logarithmic DelatDE distance estimation functions, to find which works best.</source>
        <translation type="vanished">Provare entrambe le funzioni DE, Lineare Logaritmica e DeltaDE per trovare quale funziona meglio.</translation>
    </message>
    <message>
        <source>Transform: </source>
        <translation type="vanished">Trasformazione: </translation>
    </message>
    <message>
        <source>Slot , Mag Transform Type and  Iteration controls for additional transforms</source>
        <translation type="vanished">Slot, Tipo Trasformazione Mag e controllo Iterazioni per trasformazioni addizionali</translation>
    </message>
    <message>
        <source>Mag Transform Parameters.  </source>
        <translation type="vanished">Parametri Trasformazione Mag</translation>
    </message>
    <message>
        <source>power B.z  </source>
        <translation type="vanished">potenza B.z </translation>
    </message>
    <message>
        <source>power B.x  </source>
        <translation type="vanished">potenza B.x </translation>
    </message>
    <message>
        <source>power B.y </source>
        <translation type="vanished">potenza B.y </translation>
    </message>
    <message>
        <source>power A.z  </source>
        <translation type="vanished">potenza A.z </translation>
    </message>
    <message>
        <source>Scale  z.y</source>
        <translation type="vanished">Scala z.y </translation>
    </message>
    <message>
        <source>power A.x  </source>
        <translation type="vanished">potenza A.x</translation>
    </message>
    <message>
        <source>power A.y  </source>
        <translation type="vanished">potenza A.y</translation>
    </message>
    <message>
        <source>Scale  z.z</source>
        <translation type="vanished">Scala z.z</translation>
    </message>
    <message>
        <source>Scale  z.x</source>
        <translation type="vanished">Scala z.x</translation>
    </message>
    <message>
        <source> Pine Tree  addCpixel ConstantMultiplier  ( c.y  c.z Swap)</source>
        <translation type="vanished">Pino Moltiplicatore Costanti addCpixel (scambia c.y c.z)</translation>
    </message>
    <message>
        <source>Cpixel Multiplier   </source>
        <translation type="vanished">Moltiplicatore Cpixel   </translation>
    </message>
    <message>
        <source>Pr&amp;ism Shape</source>
        <translation type="vanished">Forma a Pr&amp;isma</translation>
    </message>
    <message>
        <source>Stop at iteration :</source>
        <translation type="vanished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <source>Note: Default = 1</source>
        <translation type="vanished">Nota: Default = 1</translation>
    </message>
    <message>
        <source>Dot scale:</source>
        <translation type="vanished">Prodotto scala:</translation>
    </message>
    <message>
        <source>Shift/offset scale:  </source>
        <translation type="vanished">Scambia/offset scala:  </translation>
    </message>
    <message>
        <source>Stop Pwr2 at iteration:</source>
        <translation type="vanished">Ferma Pwr2 all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start Pwr2 at iteration :</source>
        <translation type="vanished">Inizia Pwr2 all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Pine T&amp;ree  Pwr2      addCpixel ConstantMultiplier  ( c.y c.z Swap)</source>
        <translatorcomment>really not clear, &quot;pine...&quot;</translatorcomment>
        <translation type="vanished">Pino Pwr2      addCpixel ConstantMultiplier  (Scambia c.y c.z)</translation>
    </message>
    <message>
        <source>Analytic Linear DE adjustment factor: </source>
        <translation type="vanished">Fattore controllo DE analitica lineare:</translation>
    </message>
    <message>
        <source>Final Scale:</source>
        <translation type="vanished">Scala Finale:</translation>
    </message>
    <message>
        <source>Scale A:</source>
        <translation type="vanished">Scala A:</translation>
    </message>
    <message>
        <source>Vector B:</source>
        <translation type="vanished">Vettore B:</translation>
    </message>
    <message>
        <source>Scale B:</source>
        <translation type="vanished">Scala B:</translation>
    </message>
    <message>
        <source>PI Scale:</source>
        <translation type="vanished">Scala PI:</translation>
    </message>
    <message>
        <source>PART B</source>
        <translation type="vanished">PARTE B</translation>
    </message>
    <message>
        <source>PART A</source>
        <translation type="vanished">PARTE A</translation>
    </message>
    <message>
        <source>Vector A:</source>
        <translation type="vanished">Vettore A:</translation>
    </message>
    <message>
        <source>enable Base Shape </source>
        <translation type="vanished">Abilita Forma Base   </translation>
    </message>
    <message>
        <source>Prism Shape</source>
        <translation type="vanished">Forma a Prisma</translation>
    </message>
    <message>
        <source>Stop Menger at iteration:</source>
        <translation type="vanished">Stop Menger all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start Menger at iteration :</source>
        <translation type="vanished">Inizia Menger all&apos;iterazione:</translation>
    </message>
    <message>
        <source>enable CrossMengerTrick</source>
        <translation type="vanished">abilita CrossMengerTrick  </translation>
    </message>
    <message>
        <source>Stop KIFS at iteration:</source>
        <translation type="vanished">Stop KIFS all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start KIFS at iteration :</source>
        <translation type="vanished">Inizia KIFS all&apos;iterazione:</translation>
    </message>
    <message>
        <source>enable  CrossMengerKIFS</source>
        <translation type="vanished">abilita CrossMengerKIFS  </translation>
    </message>
    <message>
        <source>Prism Temp. DE factor</source>
        <translatorcomment>&quot;Prism Temp. DE factor&quot;?</translatorcomment>
        <translation type="vanished">Fattore De Prisma Temp.</translation>
    </message>
    <message>
        <source>enable Prism Shape </source>
        <translation type="vanished">Abilita Forma a Prisma </translation>
    </message>
    <message>
        <source>Prism Shape Mod1</source>
        <translation type="vanished">Mod1 Forma Prismatica</translation>
    </message>
    <message>
        <source>Apply  z.x condition</source>
        <translation type="vanished">Applica condizione z.x</translation>
    </message>
    <message>
        <source>DE tweak: </source>
        <translation type="vanished">Ritocco DE: </translation>
    </message>
    <message>
        <source>z.y scale: </source>
        <translation type="vanished">scala z.y: </translation>
    </message>
    <message>
        <source>z.z scale: </source>
        <translation type="vanished">scala z.z: </translation>
    </message>
    <message>
        <source>Prism Shape Rotation</source>
        <translation type="vanished">Rotazione Forma Prismatica</translation>
    </message>
    <message>
        <source>enable CrossMengerTrick Mod1</source>
        <translatorcomment>strange names</translatorcomment>
        <translation type="vanished">Abilita CrossMengerTrick Mod1</translation>
    </message>
    <message>
        <source>Scale: </source>
        <translation type="vanished">Scala: </translation>
    </message>
    <message>
        <source>Multiplier</source>
        <translation type="vanished">Moltiplicatore</translation>
    </message>
    <message>
        <source>Sub Multiplier </source>
        <translation type="vanished">Sub Moltiplicatore</translation>
    </message>
    <message>
        <source>Sub Multiplier</source>
        <translation type="vanished">Sub Moltiplicatore</translation>
    </message>
    <message>
        <source>Enable Limits (box) on Rendering Engine tab,  and change bottom parameter   z: 10   to    z: 0,  and try low bailout  of 1 or 2.</source>
        <translation type="vanished">Abilita Limiti (box) sulla scheda Motore di rendering, e cambiare i parametri in fondo,  z: 10  a  z: 0, provare anche valori bassi di bailout,  1 o 2.</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Sing&amp;le Formula addCpixel  Constant Multiplier </source>
        <translation type="vanished">Moltiplicatore Costanti addCpixel Formula Singo&amp;la</translation>
    </message>
    <message>
        <source>Single Formula Julia &amp;mode</source>
        <translation type="vanished">&amp;Modo Julia e Formula Singola</translation>
    </message>
    <message>
        <source>Informa&amp;tion</source>
        <translation type="vanished">Informa&amp;zione</translation>
    </message>
    <message>
        <source>Box Fold  </source>
        <translation type="vanished">Fold Box</translation>
    </message>
    <message>
        <source>&amp;Rotation of separate folding planes</source>
        <translation type="vanished">&amp;Rotazione piani di folding separati</translation>
    </message>
    <message>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier for Mandelbox iterations</source>
        <translation type="vanished">addCpi&amp;xel Scambio Assi ConstantMultiplier per iterazioni Mandelbox</translation>
    </message>
    <message>
        <source>Men&amp;ger_Sponge</source>
        <translation type="vanished">Spugna_di_Men&amp;ger</translation>
    </message>
    <message>
        <source>  Menger sub-loop Iterations</source>
        <translation type="vanished">Sub-iterazioni Menger</translation>
    </message>
    <message>
        <source> Multiplier:    </source>
        <translation type="vanished"> Moltiplicatore:  </translation>
    </message>
    <message>
        <source>Menger Scale Factor: </source>
        <translation type="vanished">Fattore scala Menger: </translation>
    </message>
    <message>
        <source>addCpixe&amp;l Axis Swap ConstantMultiplier for Menger Sponge iterations</source>
        <translation type="vanished">addCpixe&amp;l Scambio Assi ConstantMultiplier per iterazioni Spugna Menger</translation>
    </message>
    <message>
        <source>Scale  component</source>
        <translation type="vanished">Componente Scala</translation>
    </message>
    <message>
        <source>Coloring components (beta version only)</source>
        <translation type="vanished">Componenti colorazione (solo versione beta)</translation>
    </message>
    <message>
        <source>Rotate about z axis :  </source>
        <translation type="vanished">Ruota attorno asse z:</translation>
    </message>
    <message>
        <source>Beta angle offset1:  </source>
        <translation type="vanished">offset1 angolo beta:  </translation>
    </message>
    <message>
        <source>Bulb Power:</source>
        <translation type="vanished">Potenza bulb:</translation>
    </message>
    <message>
        <source>Beta angle offset2:</source>
        <translation type="vanished">offset2 angolo beta:</translation>
    </message>
    <message>
        <source>Enable cos-tan mode (disable sin-tan2 mode)</source>
        <translation type="vanished">abilita modo cos-tan  (disabilita modo sin-tan2)</translation>
    </message>
    <message>
        <source>Phi Scale:</source>
        <translatorcomment>PI would be ambiguous</translatorcomment>
        <translation type="vanished">Scala Phi:</translation>
    </message>
    <message>
        <source>Theta Scale:</source>
        <translation type="vanished">Scala Teta:</translation>
    </message>
    <message>
        <source>Analytic &amp;Logrithmic DE adjustment </source>
        <translation type="vanished">Ritocco DE Analitico &amp;Logaritmica </translation>
    </message>
    <message>
        <source>Analytic Logrithmic DE offset1: </source>
        <translation type="vanished">Offset 1 DE Analtico Logaritmica: </translation>
    </message>
    <message>
        <source>Analytic Logrithmic DE offset2: </source>
        <translation type="vanished">Offset 2 DE Analtico Logaritmica: </translation>
    </message>
    <message>
        <source>fa&amp;bs() Controls</source>
        <translation type="vanished">Controlli fa&amp;bs()</translation>
    </message>
    <message>
        <source>Mandelbulb  Multi_Trig Formula;</source>
        <translation type="vanished">Formula Mandelbulb  Multi_Trig</translation>
    </message>
    <message>
        <source>fabs() Controls (&amp;Burning Ship mode)</source>
        <translation type="vanished">Controlli fabs() (modo &amp;Burning Ship)</translation>
    </message>
    <message>
        <source>Phi scale:</source>
        <translation type="vanished">Scala Phi:</translation>
    </message>
    <message>
        <source>Theta scale:</source>
        <translation type="vanished">Scala Teta:</translation>
    </message>
    <message>
        <source>Enable cosine mode (disable sine mode)</source>
        <translation type="vanished">Abilita modo coseno (disabilita modo seno)</translation>
    </message>
    <message>
        <source>Polar to Cartesian -  Sine Mode z.z = sin(th);</source>
        <translation type="vanished">Da polare a Cartesiano - Modo Seno z.z = sin(th);</translation>
    </message>
    <message>
        <source>in Sine mode, enable   costh = cos(th0)</source>
        <translation type="vanished">in modo Seno, abilita  costh = cos(th0)</translation>
    </message>
    <message>
        <source>in Cosine mode, enable  sinth = sin(th0)</source>
        <translation type="vanished">In modo Coseno, abilita sinth = sin(th0)</translation>
    </message>
    <message>
        <source>Menger Sponge Start Function     Fabs function z = fabs( z + const.A);</source>
        <translation type="vanished">Inizia Funzione Spugna di Menger      Funzione fabs di z = fabs( z + const.A);</translation>
    </message>
    <message>
        <source>Menger Sponge Middle Functions</source>
        <translation type="vanished">Funzioni intermedie Spugna di Menger</translation>
    </message>
    <message>
        <source>Middle &amp;Rotation  </source>
        <translation type="vanished">&amp;Rotazione Intermedia  </translation>
    </message>
    <message>
        <source>Middle &amp;Box Fold  </source>
        <translation type="vanished">Folding &amp;Box Intermedio</translation>
    </message>
    <message>
        <source>Middle FabsBoxFold (z = fabs(z  + Const.F1)  -  fabs(z - Const.F2)  -  (z * Const.F3)) * Scale</source>
        <translation type="vanished">FabsBoxFold Intermedio (z = fabs(z  + Const.F1)  -  fabs(z - Const.F2)  -  (z * Const.F3)) * Scale</translation>
    </message>
    <message>
        <source>Start scale at iteration:</source>
        <translation type="vanished">Inizia Scala all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Stop scale at iteration:</source>
        <translation type="vanished">Ferma scala all&apos;iterazione:</translation>
    </message>
    <message>
        <source>FabsBoxFold Scale </source>
        <translation type="vanished">Scala FabsBoxFold </translation>
    </message>
    <message>
        <source>Menger Sponge End Function     Apply Constants; </source>
        <translation type="vanished">Fine Funzione Spugna di Menger     Applica Costanti  </translation>
    </message>
    <message>
        <source> Offset Multiplier:    </source>
        <translation type="vanished">Moltiplicatore Offset:  </translation>
    </message>
    <message>
        <source> Menger Sponge scale:    </source>
        <translation type="vanished">Scala Spugna di Menger:   </translation>
    </message>
    <message>
        <source> Conditional mode:     if ( z.z  &lt; limit )  z.z = z.z;</source>
        <translation type="vanished">Modo condizionale:     if ( z.z  &lt; limit )  z.z = z.z;</translation>
    </message>
    <message>
        <source>Cpixel Multiplier: </source>
        <translation type="vanished">Moltiplicatore Cpixel: </translation>
    </message>
    <message>
        <source>Rotat&amp;ion</source>
        <translation type="vanished">Rotatz&amp;ione</translation>
    </message>
    <message>
        <source>Stop Rotation at iteration:</source>
        <translation type="vanished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <source>Benesi Mag Transform T&amp;1</source>
        <translation type="vanished">Trasformazione Benesi Mag T&amp;1</translation>
    </message>
    <message>
        <source>Benesi Mag Transform T&amp;1 Mod</source>
        <translation type="vanished">Trasformazione Benesi Mag T&amp;1 Mod</translation>
    </message>
    <message>
        <source>Stop T1 Mod at iteration:</source>
        <translation type="vanished">Ferma T1 Mod all&apos;iterazione:</translation>
    </message>
    <message>
        <source>Start T1 Mod at iteration :</source>
        <translation type="vanished">Inizia T1 Mod all&apos;iterazione:</translation>
    </message>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )&amp;  - (  z * const.C  )</source>
        <translation type="vanished">fABS Aggiunge Multi Costantei   z = fabs(z + const.A) - fabs(z - const.B)&amp;  - (z * const.C)</translation>
    </message>
    <message>
        <source>Enable fABS(z - B)</source>
        <translation type="vanished">Abilita fABS(z - B)</translation>
    </message>
    <message>
        <source>Enable fABS(z + A)</source>
        <translation type="vanished">Abilita fABS(z + A)</translation>
    </message>
    <message>
        <source>Men&amp;ger Sponge</source>
        <translation type="vanished">Spugna di Men&amp;ger</translation>
    </message>
    <message>
        <source>Temp DE fudge factor</source>
        <translation type="vanished">Fattore Temp fudge DE</translation>
    </message>
    <message>
        <source>Stop:</source>
        <translation type="vanished">Ferma:</translation>
    </message>
    <message>
        <source> Polynomial iterations. Start :</source>
        <translation type="vanished">Iterazione polinomiale -Inizio: </translation>
    </message>
    <message>
        <source>PART B =  ScaleB *  z  + VectorC</source>
        <translation type="vanished">PARTE B =  ScaleB *  z  + VectorC</translation>
    </message>
    <message>
        <source> fn( z)  options,  (default = z;)</source>
        <translation type="vanished">opzione fn(z), (default = z)</translation>
    </message>
    <message>
        <source>Analytic Linear DE multiplier: </source>
        <translation type="vanished">Moltiplicatore DE Lineare Analitico: </translation>
    </message>
    <message>
        <source> fn( z) = Pi Rotation about &amp;VectorA</source>
        <translation type="vanished"> fn( z) = Rotazione di Pi attorno a &amp;VectorA</translation>
    </message>
    <message>
        <source>Pi Rotation:</source>
        <translation type="vanished">Rotazione Pi:</translation>
    </message>
    <message>
        <source>Power2 Polynomial   Az^2 + Bz + C</source>
        <translation type="vanished">Power2 Polinomiale Az^2 + Bz + C</translation>
    </message>
    <message>
        <source>Vector C:</source>
        <translation type="vanished">Vettore C:</translation>
    </message>
    <message>
        <source> Final Scale:</source>
        <translation type="vanished">Scala Finale:</translation>
    </message>
    <message>
        <source>PART A   =  - (ScaleA * z + VectA)  *  fn(z)</source>
        <translation type="vanished">PARTE A   =  - (ScaleA * z + VectA)  *  fn(z)</translation>
    </message>
    <message>
        <source>Analytic Linear DE offset: </source>
        <translation type="vanished">Offest DE lineare analitico: </translation>
    </message>
    <message>
        <source>Pi Scale:</source>
        <translation type="vanished">Scala Pi:</translation>
    </message>
    <message>
        <source>Menger_Sponge</source>
        <translation type="vanished">Spugna_di_Menger</translation>
    </message>
    <message>
        <source> Menger Iterations</source>
        <translation type="vanished">Iterazioni di Menger</translation>
    </message>
    <message>
        <source>Ring number:</source>
        <translation type="vanished">Numero anelli:</translation>
    </message>
    <message>
        <source>Ring radius:</source>
        <translation type="vanished">Raggio anelli:</translation>
    </message>
    <message>
        <source>Ring thickness:</source>
        <translation type="vanished">Spessore anelli:</translation>
    </message>
    <message>
        <source>Size factor:</source>
        <translation type="vanished">fattore dimensioni: </translation>
    </message>
    <message>
        <source>P&amp;re-Scale</source>
        <translation type="vanished">P&amp;re-Scala</translation>
    </message>
    <message>
        <source>Pre-Scale</source>
        <translation type="vanished">Pre-Scala</translation>
    </message>
    <message>
        <source>Theta power</source>
        <translation type="vanished">Potenza Teta</translation>
    </message>
    <message>
        <source>Radius power</source>
        <translation type="vanished">Potenza raggio</translation>
    </message>
    <message>
        <source>Phi power</source>
        <translation type="vanished">Potenza Phi</translation>
    </message>
    <message>
        <source>Analytic &amp;Logrithmic DE adjustment ( beta version only, may need to force Delta DE method)</source>
        <translation type="vanished">Ritocco DE Analitica &amp;Logaritmica (solo versione beta, potrebbe essere necessario usare il metodo Forza Delta DE)</translation>
    </message>
    <message>
        <source>enable aux.r_dz squared</source>
        <translation type="vanished">Abilita aux.r_dz al quadrato</translation>
    </message>
    <message>
        <source>Spherica&amp;l Offset</source>
        <translation type="vanished">Offset &amp;Sferico</translation>
    </message>
    <message>
        <source>Offset Radius:   </source>
        <translation type="vanished">Raggio Offset:  </translation>
    </message>
    <message>
        <source>Th0 Scale</source>
        <translation type="vanished">Scala Th0</translation>
    </message>
    <message>
        <source>Polar to Cartesian -  Sine Mode z.z = sin(th0);</source>
        <translation type="vanished">Da Polare a Cartesiano -  Modo Seno z.z = sin(th0)</translation>
    </message>
    <message>
        <source>Ph0 Scale</source>
        <translation type="vanished">Scala Ph0</translation>
    </message>
    <message>
        <source>Global Constant Multiplier of c.z must be set at 0.0 for this formula</source>
        <translation type="vanished">Moltiplicatore Costanti globale di c.z, dev&apos;essere impostato a 0 per questa formula</translation>
    </message>
    <message>
        <source>addCpixel Symmetrical  Constant &amp;Multiplier </source>
        <translation type="vanished">addCpixel Symmetrical  Constante &amp;Moltiplicatore </translation>
    </message>
    <message>
        <source>addYZ Scale : </source>
        <translation type="vanished">Scala addXZ: </translation>
    </message>
    <message>
        <source>addCpixel Symmetrical Constant &amp;Multiplier </source>
        <translation type="vanished">&amp;Moltipicatore costanti simmetrico addCpixel</translation>
    </message>
    <message>
        <source>&amp;Quaternion Fold</source>
        <translation type="vanished">Folding &amp;Quaternione</translation>
    </message>
    <message>
        <source>Enable DE Vector mode</source>
        <translation type="vanished">Abilita modo Vettori DE</translation>
    </message>
    <message>
        <source>DE Vector Influence:</source>
        <translation type="vanished">DE Influenza Vettore:</translation>
    </message>
    <message>
        <source>Scale  </source>
        <translation type="vanished">Scala  </translation>
    </message>
    <message>
        <source>Note 1) : the pseudo kleinian fractal on this UI, works with the standard linear analytic DE calculation.</source>
        <translation type="vanished">Nota 1) : il frattale pseudo kleinian in questa UI, lavora utilizzando il calcolo DE di tipo standard, analitico e lineare</translation>
    </message>
    <message>
        <source>Note 2) : termination condition is when iteration reaches  Maxiter,  </source>
        <translation type="vanished">Nota 2): la condizione termina quando il numero delle iterazioni è uguale al numero Maxiter  </translation>
    </message>
    <message>
        <source>Pseu&amp;do Kleinian Variation</source>
        <translation type="vanished">Variante Pseu&amp;do Kleinian</translation>
    </message>
    <message>
        <source>Pseudo Kleinian Parameters</source>
        <translation type="vanished">Parametri Pseudo Kleinian</translation>
    </message>
    <message>
        <source>Size R2:</source>
        <translation type="vanished">Dimensioni R2:</translation>
    </message>
    <message>
        <source>DEadd (tweak): </source>
        <translation type="vanished">DEadd (messa a punto)</translation>
    </message>
    <message>
        <source>z subtr. scale:</source>
        <translation type="vanished">scala zubtr:</translation>
    </message>
    <message>
        <source>Note 1) : termination condition is when iteration reaches  Maxiter,  </source>
        <translation type="vanished">Nota 1): la condizione di arresto si verifica quando il numero di iterazioni è uguale al numero di Maxiter  </translation>
    </message>
    <message>
        <source>Note 2) : each additional iteration adds more detail.</source>
        <translation type="vanished">Nota 2): ogni iterazione addizionale incrementa il livello di dettaglio</translation>
    </message>
    <message>
        <source>Note 3 ): this type of DE calculation will not work with other types.</source>
        <translation type="vanished">Nota 3): questo tipo di calcolo DE non funzionerà su tipi differenti</translation>
    </message>
    <message>
        <source>Bo&amp;x Fold</source>
        <translation type="vanished">Folding Bo&amp;x</translation>
    </message>
    <message>
        <source>Note 2) : each additional iteration adds more detail.  </source>
        <translation type="vanished">Nota 2): ogni iterazione addizionale incrementa il livello di dettaglio</translation>
    </message>
    <message>
        <source>Note 3) : this type of DE calculation will not work with other types.</source>
        <translation type="vanished">Nota 3): questo tipo di calcolo DE non funzionerà su tipi differenti</translation>
    </message>
    <message>
        <source>z.z  subtr. scale:</source>
        <translation type="vanished">z.z  subtr. scala:</translation>
    </message>
    <message>
        <source>In&amp;ternal Rotation; </source>
        <translation type="vanished">Rotazione In&amp;terna</translation>
    </message>
    <message>
        <source>Quaternion Scales    </source>
        <translation type="vanished">Scale Quaternioni </translation>
    </message>
    <message>
        <source>Sq-Multiplier</source>
        <translation type="vanished">Sq-Moltiplicatore</translation>
    </message>
    <message>
        <source>Use bailout between 1.0 and 2.4</source>
        <translation type="vanished">Usare bailout tra 1.0 e 2.4</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="505"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="142"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="586"/>
        <source>z.y:</source>
        <translation></translation>
    </message>
    <message>
        <source>Sine Function  Scale</source>
        <translation type="vanished">Scala Funzione Seno</translation>
    </message>
    <message>
        <source>Periodic Cubic Tiling</source>
        <translatorcomment>not sure</translatorcomment>
        <translation type="vanished">Copertura pattern periodico cubico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="216"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="209"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="434"/>
        <source>z.z:</source>
        <translation></translation>
    </message>
    <message>
        <source>Sine function</source>
        <translation type="vanished">Funzione Seno</translation>
    </message>
    <message>
        <source>Radius^2 &lt; Minimum Radius^2 Parameters</source>
        <translation type="vanished">Parametri Raggio^2 &lt; Raggio minimo^2</translation>
    </message>
    <message>
        <source> Scale (not smooth inside)</source>
        <translatorcomment>strange translation</translatorcomment>
        <translation type="vanished">Scala (modalità rigida interna)</translation>
    </message>
    <message>
        <source>Second Offset</source>
        <translation type="vanished">Secondo Offset</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional4D.ui" line="202"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="469"/>
        <location filename="../qt_data/fractal_transf_reciprocal4D.ui" line="515"/>
        <source>z.x:</source>
        <translation></translation>
    </message>
    <message>
        <source>Second Scale</source>
        <translation type="vanished">Seconda Scala</translation>
    </message>
    <message>
        <source>Fudge Factor</source>
        <translation type="vanished">Fattore Fudge</translation>
    </message>
    <message>
        <source>First Offset</source>
        <translation type="vanished">Primo Offset</translation>
    </message>
    <message>
        <source>First Scale</source>
        <translation type="vanished">Prima Scala</translation>
    </message>
    <message>
        <source>Radius^2 &gt; Minimum Radius^2 Parameters</source>
        <translation type="vanished">Parametri Raggio^2 &lt; Raggio minimo^2</translation>
    </message>
    <message>
        <source>Smooth Inside</source>
        <translation type="vanished">modalità non rigida interna</translation>
    </message>
    <message>
        <source>Riemann Sphere Msltoe </source>
        <translation type="vanished">Sfera di Riemann Msltoe </translation>
    </message>
    <message>
        <source>Scale and Offsets:</source>
        <translation type="vanished">Scala e Offset:</translation>
    </message>
    <message>
        <source>Riemann Sphere Msltoe Variation1</source>
        <translation type="vanished">Sfera di Riemann Msltoe  Variante1</translation>
    </message>
    <message>
        <source>Constant Multiplier</source>
        <translation type="vanished">Moltiplicatore Costanti</translation>
    </message>
    <message>
        <source>Cpixel  Multiplier  </source>
        <translation type="vanished">Moltiplicatore Cpixel  </translation>
    </message>
    <message>
        <source>addCpixel c.x c.y axis swap ConstantMultiplier</source>
        <translation type="vanished">addCpixel c.x c.y Scambio Assi ConstantMultiplier</translation>
    </message>
    <message>
        <source>c.x c.y swap enabled</source>
        <translation type="vanished">Scambio c.x c.y abilitato</translation>
    </message>
    <message>
        <source>alternate c.x c.y swap</source>
        <translation type="vanished">alterna scambio c.x c.y</translation>
    </message>
    <message>
        <source>addCpixel multiplier </source>
        <translation type="vanished">Moltiplicatore addCpixel </translation>
    </message>
    <message>
        <source>Variable</source>
        <translation type="vanished">Variabile</translation>
    </message>
    <message>
        <source>Addition Constant 1  - Julia Constant</source>
        <translation type="vanished">Aggiunta Costante 1 - Costante di Julia</translation>
    </message>
    <message>
        <source>Julia constant</source>
        <translation type="vanished">Costante di Julia</translation>
    </message>
    <message>
        <source>Get Point Coodinates with mouse pointer, then use right mouse click copy /paste vector to enter Julia constant.</source>
        <translation type="vanished">Cattura Coodinate tramite puntatore del mouse, poi usa la funzione copia/incolla del tasto destro del mouse per incollare i valori relativi alla costante di Julia</translation>
    </message>
    <message>
        <source>Addition Constant 2 - Offset</source>
        <translation type="vanished">Aggiunta Costante 2 - Costante di Julia</translation>
    </message>
    <message>
        <source>Offset. Use in normal mode or use to explore in Julia mode</source>
        <translation type="vanished">Offset Da usare in modo normale o per esplorare modalità Julia</translation>
    </message>
    <message>
        <source>Mag Transform &amp;T1Mod   Internal Rotation; </source>
        <translation type="vanished">Trasformazione Mag &amp;T1Mod   Rotazione Interna  </translation>
    </message>
    <message>
        <source>Mag Transform &amp;T2  Internal Rotation; </source>
        <translation type="vanished">Trasformazione Mag &amp;T2  Rotazione Interna  </translation>
    </message>
    <message>
        <source>Mag Transform &amp;T3  Internal Rotation; </source>
        <translation type="vanished">Trasformazione Mag &amp;T3  Rotazione Interna  </translation>
    </message>
    <message>
        <source>Mag Transform &amp;T4 Internal Rotation; </source>
        <translation type="vanished">Trasformazione Mag &amp;T4  Rotazione Interna  </translation>
    </message>
    <message>
        <source>Power A  </source>
        <translation type="vanished">Potenza A  </translation>
    </message>
    <message>
        <source>Power B  </source>
        <translation type="vanished">Potenza B  </translation>
    </message>
    <message>
        <source>Mag Transform &amp;T5b  Internal Rotation; </source>
        <translation type="vanished">Trasformazione Mag &amp;T5b  Rotazione Interna  </translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Cube to Sphere transform&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;Description:Warpsacubetoasphere;transformmadebyM.Benesi,optimizedbyLuca&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;http://www.fractalforums.com/mathematics/circle2square/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>English version all wrong (no space)</translatorcomment>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Trasformazione Cubo in Sfera&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;Descrizione: Cambia la forma di un oggetto cubico  in quella di un oggetto sferico, formula di M.Benesi, ottimizzata da Dark Beam&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;http://www.fractalforums.com/mathematics/circle2square/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>z.z  scale : </source>
        <translation type="vanished">Scala z.z: </translation>
    </message>
    <message>
        <source>Folding Limit  </source>
        <translation type="vanished">Limite Folding</translation>
    </message>
    <message>
        <source>Folding Value  </source>
        <translation type="vanished">Valore Folding</translation>
    </message>
    <message>
        <source>Box Offset  </source>
        <translation type="vanished">Offset Box  </translation>
    </message>
    <message>
        <source>fABS Add Constant  z = fabs( z + constant)</source>
        <translation type="vanished">fABS Aggiunge Costante  z = fabs( z + constant)</translation>
    </message>
    <message>
        <source>fABS addition constant   </source>
        <translation type="vanished">fABS aggiunta costante    </translation>
    </message>
    <message>
        <source>fABS Add Constant V2  z = fabs( z + constant) - fabs(  z - constant) - z;</source>
        <translation type="vanished">fABS Aggiunge Costante V2  z = fabs(z + constant) - fabs(z - constant) - z;</translation>
    </message>
    <message>
        <source>fABS Add Constant V2  4D     z = fabs( z + constant) - fabs(  z - constant) - z;</source>
        <translation type="vanished">fABS Aggiunge Costante V2  4D     z = fabs(z + constant) - fabs(z - constant) - z;</translation>
    </message>
    <message>
        <source>fABS Add Constant  4D        z = fabs( z + constant)</source>
        <translation type="vanished">fABS Aggiunge Costante  4D        z = fabs(z + constant)</translation>
    </message>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )  - (  z * const.C  )</source>
        <translation type="vanished">fABS Aggiunge Multi Costante   z = fabs(z + const.A) - fabs(z - const.B)  - (z * const.C)</translation>
    </message>
    <message>
        <source>1st half tetrahedral symmetry planes:</source>
        <translation type="vanished">Piani simmetria tetraedro prima metà:</translation>
    </message>
    <message>
        <source>2nd half tetrahedral symmetry planes:</source>
        <translation type="vanished">Piani simmetria tetraedro seconda metà:</translation>
    </message>
    <message>
        <source>Enable newZy = z.x * sin(z.y)</source>
        <translation type="vanished">Abilita newZy = z.x * sin(z.y)</translation>
    </message>
    <message>
        <source>Enable newZx = z.x * cos(z.y)</source>
        <translation type="vanished">Abilita newZx = z.x * cos(z.y)</translation>
    </message>
    <message>
        <source>Analytic DE Muliplier: </source>
        <translation type="vanished">Moltiplicatore DE analitica:</translation>
    </message>
    <message>
        <source>Analytic DE Offset: </source>
        <translation type="vanished">Offset DE Analitica: </translation>
    </message>
    <message>
        <source> Iteration Weight.;  z  =  (  z * const.Z) + (  zA * const.A) + ( zB * const.B)</source>
        <translation type="vanished">Peso Iterazione z  =  (z * const.Z) + (zA * const.A) + (zB * const.B)</translation>
    </message>
    <message>
        <source> Iteration Weight 4D   z  =  (  z * const.Z) + (  zA * const.A) + ( zB * const.B)</source>
        <translation type="vanished">Peso iterazione 4D   z  =  (z * const.Z) + (zA * const.A) + (zB * const.B)</translation>
    </message>
    <message>
        <source>CX Cpixel  Multiplier  </source>
        <translation type="vanished">Moltiplicatore CX Cpixel </translation>
    </message>
    <message>
        <source>CY Cpixel  Multiplier  </source>
        <translation type="vanished">Moltiplicatore CY Cpixel </translation>
    </message>
    <message>
        <source>CZ Cpixel  Multiplier  </source>
        <translation type="vanished">CY Cpixel CZ Cpixel </translation>
    </message>
    <message>
        <source>Suggestion:
=You can use it for example as a pre-transform for bulbs or boxes.</source>
        <translation type="vanished">Suggerimento:
Puoi usarlo ad esempio come pre-transformazione per mandelbulb o mandelbox.</translation>
    </message>
    <message>
        <source>&amp;Menger Sponge Start Function     Fabs function z = fabs( z + const.A);</source>
        <translation type="vanished">Inizia Funzione Spugna di &amp;Menger     Fabs funzione z = fabs(z + const.A) </translation>
    </message>
    <message>
        <source>&amp;Menger Sponge End Function     Apply Constants; </source>
        <translation type="vanished">Fine Funzione Spugna di &amp;Menger     Applica Costanti  </translation>
    </message>
    <message>
        <source>Angle multiplication:</source>
        <translation type="vanished">Moltiplicazione angolo:</translation>
    </message>
    <message>
        <source>Neg fABS Add Constant  z =  - fabs( z + constant)</source>
        <translation type="vanished">Neg fABS Aggiunfi Costante  z =  - fabs(z + constant)</translation>
    </message>
    <message>
        <source>Addition constant   </source>
        <translation type="vanished">Aggiunta Costante   </translation>
    </message>
    <message>
        <source>fn(z)  = Pi Rotation about Vector A</source>
        <translation type="vanished">fn(z)  = Rotazione Pi attorno al vettore A</translation>
    </message>
    <message>
        <source>Enable Sca&amp;le and Offset controls; (  iterates only the quaternion fold when disabled.)</source>
        <translation type="vanished">Abilita controlli Sca&amp;la e Offset  (itera soltanto il Folding del quaternione quando disabilitato)</translation>
    </message>
    <message>
        <source>Scale:    </source>
        <translation type="vanished">Scala:  </translation>
    </message>
    <message>
        <source>DE Vector Influence: </source>
        <translation type="vanished">DE Influenza Vettore:</translation>
    </message>
    <message>
        <source>Analytical Linear DE adjustment offset</source>
        <translation type="vanished">Ritocco offset DE Lineare Analitica</translation>
    </message>
    <message>
        <source>Rotation of separate folding planes</source>
        <translation type="vanished">Rotazione Piani di Folding separati</translation>
    </message>
    <message>
        <source>alpha: </source>
        <translation type="vanished">alfa: </translation>
    </message>
    <message>
        <source>Variable    </source>
        <translation type="vanished">Variabile  </translation>
    </message>
    <message>
        <source>Scale Variable:</source>
        <translation type="vanished">Variabile Scala:</translation>
    </message>
    <message>
        <source> SphereInv from M3D</source>
        <translation type="vanished">SphereInv presa da M3D</translation>
    </message>
    <message>
        <source>Pre_scale</source>
        <translation type="vanished">Pre_Scala</translation>
    </message>
    <message>
        <source>Pre_dot(z,z)</source>
        <translation type="vanished">Pre_Prodotto(z,z)</translation>
    </message>
    <message>
        <source>No mode =  unconditional spherical inversion  z = z / r2;</source>
        <translation type="vanished">no modale = inversione sferica senza condizione z = z / r2;</translation>
    </message>
    <message>
        <source>let r2 = 2 * Radius2 - r2 (non conformal &quot;reversal&quot; inversion) </source>
        <translation type="vanished">r2 = 2 * Radius2 - r2 (inversione non conforme &quot;rovesciata&quot;) </translation>
    </message>
    <message>
        <source>Mode Radius2:</source>
        <translation type="vanished">Modo Radius2:</translation>
    </message>
    <message>
        <source>Mode Radius1:</source>
        <translation type="vanished">Modo Radius1:</translation>
    </message>
    <message>
        <source>Mode 1</source>
        <translation type="vanished">Modo 1</translation>
    </message>
    <message>
        <source>let r2 = Radius2  (conformal - undo the inversion inside a sphere) </source>
        <translation type="vanished"> r2 = Radius2  (conforme - annulla l&apos;inversione all&apos;interno di una sfera) </translation>
    </message>
    <message>
        <source>Mode 2</source>
        <translation type="vanished">Modo 2</translation>
    </message>
    <message>
        <source>Adjust Offset :      </source>
        <translation type="vanished">Aggiusta Offset: </translation>
    </message>
    <message>
        <source>Suggestion:
Use as pre-transform for one iteration, 
will invert the complete coord system.
Cut off at Z=0 to see the inside.
</source>
        <translation type="vanished">Suggerimento:
Da usare una volta (un&apos;iterazione) come pre.trasformazione 
Invertirà completamente il sistema di coordinate
Sezionare trasversalmente a z=0 ,per poter vedere l&apos;interno</translation>
    </message>
    <message>
        <source> Spherical Fold - ABox/ASurf</source>
        <translation type="vanished">Folding Sferico- ABox/ASurf</translation>
    </message>
    <message>
        <source> Spherical Fold, Variable Radius Controls -  MBox type</source>
        <translation type="vanished">Folding Sferico Controllo Raggio Variabile - tipo MBox</translation>
    </message>
    <message>
        <source>Fixed radius variable:</source>
        <translatorcomment>Fixed and variable together?</translatorcomment>
        <translation type="vanished">Variabile raggio fisso:</translation>
    </message>
    <message>
        <source>Minimum radius variable:</source>
        <translation type="vanished">Variabile raggio minimo:</translation>
    </message>
    <message>
        <source> Spherical Power Fold</source>
        <translation type="vanished">Folding Potenza Sferico</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Based on amazing_surf_mod1  from Mandelbulb3D&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Baseata sulla formula amazing_surf_mod1 da Mandelbulb3D&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>function order of z.x, z.y and z.z: </source>
        <translation type="vanished">ordine funzione z.x, z.y e z.z:</translation>
    </message>
</context>
<context>
    <name>aboxMod1</name>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="75"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="154"/>
        <source>FoldM z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="161"/>
        <source>Fold:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="250"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="257"/>
        <source>Scale Vary:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="264"/>
        <source>FoldM y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="340"/>
        <source>FoldM x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod1.ui" line="440"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>aboxMod2</name>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="65"/>
        <source>Fold x:</source>
        <translation type="unfinished">Fold x:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="141"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="240"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="247"/>
        <source>Cylinder Half Size:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Cylinder Half Size</source>
        <translation type="obsolete">Dimensioni Metà Cilindro </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="289"/>
        <source>Fold z:</source>
        <translation type="unfinished">Fold z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="371"/>
        <source>Fold y:</source>
        <translation type="unfinished">Fold y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxMod2.ui" line="380"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>aboxModKali</name>
    <message>
        <location filename="../qt_data/fractal_aboxModKali.ui" line="141"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKali.ui" line="189"/>
        <source>Offset y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKali.ui" line="290"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKali.ui" line="297"/>
        <source>Offset z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKali.ui" line="304"/>
        <source>Offset x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKali.ui" line="313"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>aboxModKaliEiffie</name>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="155"/>
        <source>Box Fold offsets</source>
        <translation type="unfinished">Offset Folding Box</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="191"/>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="724"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="207"/>
        <source>&amp;z.z box fold scale, for normal use set &quot;Offset z&quot; (above) to 0.00</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="255"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="262"/>
        <source>Stop at Iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="302"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="444"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="451"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="490"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="564"/>
        <source>Swap z.x z.z</source>
        <translation type="unfinished">Scambia z.x z.z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="599"/>
        <source>Addition Constant - Offset</source>
        <translation type="unfinished">Aggiunta Costante - Offset</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="638"/>
        <source>Minimum Radius^2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="839"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="842"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="871"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="937"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1049"/>
        <source>Cpixel Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1121"/>
        <source>&amp;Rotation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1163"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1571"/>
        <source>Scale component:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1762"/>
        <source>Orbit trap component:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1196"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1408"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1435"/>
        <source>Fold component</source>
        <translation type="unfinished">Componente Folding</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1442"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1477"/>
        <source>Fold function components -group weight:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1578"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1635"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1664"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1705"/>
        <source>Initial Color Min:</source>
        <translation type="unfinished">Colore Min iniziale:</translation>
    </message>
    <message>
        <source>Orbit trap component</source>
        <translation type="obsolete">Componente orbit trap</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1775"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Hint: change coloring algorithm from Standard in the Material Editor&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Consiglio: cambiare algoritmo colore in Standard nell&apos;editor materiali&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="1806"/>
        <source>Absolute value of z component:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="2004"/>
        <source>Coloring components (beta version only, does not work in hybrid mode)</source>
        <translation type="unfinished">Componenti colorazione (solo versione beta, non funziona in modalità ibridi)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxModKaliEiffie.ui" line="2013"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>aboxVS_icen1</name>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="40"/>
        <source>Scale Vary:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="80"/>
        <source>Fold z:</source>
        <translation type="unfinished">Fold z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="66"/>
        <source>Fold x:</source>
        <translation type="unfinished">Fold x:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="165"/>
        <source>Fold y:</source>
        <translation type="unfinished">Fold y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="329"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="417"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="424"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="431"/>
        <source>Enable internal addCpixel constant / Julia constant swap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="449"/>
        <source>Single Formula addCpixel Constant Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="549"/>
        <source>Cpixel Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="738"/>
        <source>Julia constant:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Enable   internal addCpixel  constant / Julia constant swap</source>
        <translation type="obsolete">Abilita scambio interno tra le costanti addCpixel/Julia </translation>
    </message>
    <message>
        <source>Single Formula addCpixel  Constant Multiplier</source>
        <translation type="obsolete">Formula singola - Moltiplicatore costante addCpixel</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="656"/>
        <source>Single Formula Julia mode</source>
        <translation type="unfinished">Formula singola - modo Julia</translation>
    </message>
    <message>
        <source>Julia constant</source>
        <translation type="obsolete">Costante di Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aboxVS_icen1.ui" line="875"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>abox_4D</name>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="193"/>
        <source>Parabolic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="207"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="221"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="383"/>
        <source>Limit:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="491"/>
        <source>Spherical Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="526"/>
        <source>R power:</source>
        <translation type="unfinished">Potenza R:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="200"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="961"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="987"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1029"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1036"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="561"/>
        <source>MaximumR2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="629"/>
        <source>Spherical Fold Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="854"/>
        <source>MinimumR2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1491"/>
        <source>Addition Constant:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1636"/>
        <source>Coloring parameters</source>
        <translation type="unfinished">Parametri colorazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1662"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1747"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1904"/>
        <source>Absolute value of z:</source>
        <translation type="unfinished">Valore assoluto di z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1939"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1968"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="1975"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_abox_4D.ui" line="2009"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>aexion</name>
    <message>
        <location filename="../qt_data/fractal_aexion.ui" line="90"/>
        <source>c add</source>
        <translation type="unfinished">aggiungi c</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion.ui" line="99"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>aexion_octopus_mod</name>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="75"/>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="182"/>
        <source>Internal &amp;Rotation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="404"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="568"/>
        <source>Sqr multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="580"/>
        <source>addCpi&amp;xel Symmetrical Constant Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="750"/>
        <source>Cpixel Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Enable fabs(z. x)</source>
        <translation type="obsolete">Abilita fabs(z.x)</translation>
    </message>
    <message>
        <source>Enable fabs( z.z)</source>
        <translation type="obsolete">Abilita fabs( z.z)</translation>
    </message>
    <message>
        <source>Enable fabs (z.y)</source>
        <translation type="obsolete">Abilita fabs( z.y)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="561"/>
        <source>XY multiplier:</source>
        <translation type="unfinished">moltiplicatore XY:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="577"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_aexion_octopus_mod.ui" line="822"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>amazing_surf</name>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="40"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="47"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="136"/>
        <source>Fold y:</source>
        <translation type="unfinished">Fold y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="199"/>
        <source>Fold x:</source>
        <translation type="unfinished">Fold x:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="375"/>
        <source>Scale and fold influence:</source>
        <translation type="unfinished">Scala e influenza su FoldIng:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="382"/>
        <source>Force Cylindrical Fold</source>
        <translation type="unfinished">Forza Folding cilindrico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="389"/>
        <source>Scale Vary:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="398"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="401"/>
        <source>addCpi&amp;xel ConstantMultiplier c.x c.y swap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>addCpi&amp;xel  ConstantMultiplier c.x c.y swap</source>
        <translation type="obsolete">addCpi&amp;xel  ConstantMultiplier scambio c.x c.y </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="472"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="628"/>
        <source>Rotation</source>
        <translation type="unfinished">Rotazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf.ui" line="795"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>amazing_surf_mod1</name>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="40"/>
        <source>Type 1</source>
        <translation type="unfinished">Tipo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="47"/>
        <source>Fold z.x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="61"/>
        <source>Type 2</source>
        <translation type="unfinished">Tipo 2</translation>
    </message>
    <message>
        <source>Type 4 Fold Value</source>
        <translation type="obsolete">Tipo 4 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="95"/>
        <source>Fold Type</source>
        <translation type="unfinished">Tipo Fold</translation>
    </message>
    <message>
        <source>Type 5 Fold2 Value</source>
        <translation type="obsolete">Tipo 5 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="116"/>
        <source>Type 3</source>
        <translation type="unfinished">Tipo 3</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="164"/>
        <source>Type 5</source>
        <translation type="unfinished">Tipo 5</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="82"/>
        <source>Type 4 Fold value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="102"/>
        <source>Type 5 Fold2 value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="178"/>
        <source>Type 4</source>
        <translation type="unfinished">Tipo 4</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="252"/>
        <source>Fold z.y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="328"/>
        <source>Parameters</source>
        <translation type="unfinished">Parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="356"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="548"/>
        <source>Sqrt Minimum Radius:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="608"/>
        <source>Scale and fold influence:</source>
        <translation type="unfinished">Scala e influenza su FoldIng:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="615"/>
        <source>Scale Vary:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="675"/>
        <source>Force Cylindrical Fold</source>
        <translation type="unfinished">Forza Folding cilindrico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="682"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="975"/>
        <source>Rotation</source>
        <translation type="unfinished">Rotazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1027"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1054"/>
        <source>Fold component</source>
        <translation type="unfinished">Componente Folding</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1061"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1096"/>
        <source>Fold function components -group weight:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1190"/>
        <source>Scale component:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1381"/>
        <source>Orbit trap component:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1425"/>
        <source>Absolute value of z component:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1197"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1254"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1283"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1324"/>
        <source>Initial Color Min:</source>
        <translation type="unfinished">Colore Min iniziale:</translation>
    </message>
    <message>
        <source>Orbit trap component</source>
        <translation type="obsolete">Componente orbit trap</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1394"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Hint: change coloring algorithm from Standard in the Material Editor&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Consiglio: cambiare algoritmo colore in Standard nell&apos;editor materiali&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1623"/>
        <source>Coloring components (beta version only, does not work in hybrid mode)</source>
        <translation type="unfinished">Componenti colorazione (solo versione beta, non funziona in modalità ibridi)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_mod1.ui" line="1632"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>amazing_surf_multi</name>
    <message>
        <source>Type 5 Fold2 Value</source>
        <translation type="obsolete">Tipo 5 Valore di Fold</translation>
    </message>
    <message>
        <source>Fold Type 2</source>
        <translation type="obsolete">Folding tipo 2</translation>
    </message>
    <message>
        <source>Fold Type 3</source>
        <translation type="obsolete">Folding tipo 3</translation>
    </message>
    <message>
        <source>Fold Type 4</source>
        <translation type="obsolete">Folding tipo 4</translation>
    </message>
    <message>
        <source>Type 4 Fold Value</source>
        <translation type="obsolete">Tipo 4 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="235"/>
        <source>Fold Types</source>
        <translation type="unfinished">Tipi Folding</translation>
    </message>
    <message>
        <source>Fold Type 1</source>
        <translation type="obsolete">Folding tipo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="256"/>
        <source>Fold z.y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="282"/>
        <source>Fold z.x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="314"/>
        <source>Fold Parameters</source>
        <translation type="unfinished">Parametri di Folding</translation>
    </message>
    <message>
        <source>Fold Type 5</source>
        <translation type="obsolete">Folding tipo 5</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="369"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="401"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="433"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="465"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="521"/>
        <source>type 1</source>
        <translation type="unfinished">tipo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="374"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="406"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="438"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="470"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="526"/>
        <source>type 2</source>
        <translation type="unfinished">tipo 2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="379"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="411"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="443"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="475"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="531"/>
        <source>type 3</source>
        <translation type="unfinished">tipo 3</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="384"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="416"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="448"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="480"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="536"/>
        <source>type 4</source>
        <translation type="unfinished">tipo 4</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="389"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="421"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="453"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="485"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="541"/>
        <source>type 5</source>
        <translation type="unfinished">tipo 5</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="493"/>
        <source>Slot 4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="500"/>
        <source>order of Fold Types</source>
        <translation type="unfinished">ordine tipi di Folding</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="549"/>
        <source>Slot 3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="556"/>
        <source>Slot 5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="563"/>
        <source>Slot 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="586"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="721"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="728"/>
        <source>Enable slot sequence:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Slot , Fold Type and  Iteration controls</source>
        <translation type="obsolete">Slot, Tipo Folding e controlli Iterazioni</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="748"/>
        <source>Slot 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="778"/>
        <source>z.z Fold Value</source>
        <translation type="unfinished">Valore di folding z.z</translation>
    </message>
    <message>
        <source>enable z.z  scale</source>
        <translation type="obsolete">abilita scala z.z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="40"/>
        <source>Type 5 Fold2 value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="47"/>
        <source>Type 2</source>
        <translation type="unfinished">Tipo 2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="54"/>
        <source>Type 3</source>
        <translation type="unfinished">Tipo 3</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="61"/>
        <source>Type 4</source>
        <translation type="unfinished">Tipo 4</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="104"/>
        <source>Type 4 Fold value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="249"/>
        <source>Type 1</source>
        <translation type="unfinished">Tipo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="321"/>
        <source>Type 5</source>
        <translation type="unfinished">Tipo 5</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="741"/>
        <source>Slot , Fold Type and Iteration controls</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="785"/>
        <source>enable z.z scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="905"/>
        <source>z.z Fold Limit</source>
        <translation type="unfinished">Limite folding z.z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="926"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1120"/>
        <source>Standard &amp;Functions Enabled</source>
        <translation type="unfinished">&amp;Funzioni standard abilitate</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1184"/>
        <source>Force Cylindrical Fold</source>
        <translation type="unfinished">Forza Folding cilindrico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1219"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1901"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1249"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1431"/>
        <source>Start Fold at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1263"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1385"/>
        <source>Stop Fold at iteration:</source>
        <translation type="unfinished">Ferma Folding all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1283"/>
        <source>Abox spherical folding:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1345"/>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1497"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1424"/>
        <source>Mbox spherical folding:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1468"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1590"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1820"/>
        <source>Stop Scale at iteration:</source>
        <translation type="unfinished">Ferma scala all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1859"/>
        <source>Start Scale at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1983"/>
        <source>Scale and fold influence:</source>
        <translation type="unfinished">Scala e influenza su FoldIng:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="1999"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2002"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2031"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2097"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2209"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2274"/>
        <source>&amp;Main rotation</source>
        <translation type="unfinished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2313"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2949"/>
        <source>Absolute value of z component</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2333"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2551"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2578"/>
        <source>Fold component</source>
        <translation type="unfinished">Componente Folding</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2585"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2620"/>
        <source>Fold function components -group weight:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2714"/>
        <source>Scale component</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2721"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2778"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2807"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2848"/>
        <source>Initial Color Min:</source>
        <translation type="unfinished">Colore Min iniziale:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2905"/>
        <source>Orbit trap component</source>
        <translation type="unfinished">Componente orbit trap</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="2918"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Hint: change coloring algorithm from Standard in the Material Editor&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Consiglio: cambiare algoritmo colore in Standard nell&apos;editor materiali&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="3147"/>
        <source>Coloring components (beta version only, does not work in hybrid mode)</source>
        <translation type="unfinished">Componenti colorazione (solo versione beta, non funziona in modalità ibridi)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_amazing_surf_multi.ui" line="3156"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>benesi</name>
    <message>
        <location filename="../qt_data/fractal_benesi.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi.ui" line="42"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>benesi_T1_pine_tree</name>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="35"/>
        <source>This formula does not use the global addCpixel constant multiplier (on Options tab)</source>
        <translation type="unfinished">Questa formula non usa il moltiplicatore di costante a livello globale (su scheda Opzioni)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="52"/>
        <source>Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Mag  T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="94"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="127"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="341"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="396"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="761"/>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1023"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;In this formula the c.y and c.z components of &amp;quot;CVector3 c&amp;quot; are swapped before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Pine T&amp;ree  addCpixel ConstantMultiplier  ( c.y c.z Swap)</source>
        <translation type="obsolete">Pino Moltiplicatore Costante addCpixel (Scambia c.y c.z)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="453"/>
        <source>Mag Transform &amp;T1 Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="764"/>
        <source>Pine T&amp;ree addCpixel ConstantMultiplier ( c.y c.z Swap)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="806"/>
        <source>Stop PT at iteration:</source>
        <translation type="unfinished">Ferma PT all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="839"/>
        <source>Start PT at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="848"/>
        <source>alternate c.y c.z swap</source>
        <translation type="unfinished">Alterna scambio c.y c.z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="860"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="896"/>
        <source>cx:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1026"/>
        <source>In&amp;verse_Cylindrical</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1068"/>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1349"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1101"/>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1382"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1143"/>
        <source>z.y Multiplier::</source>
        <translation type="unfinished">Moltiplicatore z.y  </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1150"/>
        <source>z scale:</source>
        <translation type="unfinished">scala z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1157"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1307"/>
        <source>Post Off&amp;set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1444"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_T1_pine_tree.ui" line="1606"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>benesi_mag_transforms</name>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="35"/>
        <source>This formula does not use the global addCpixel constant multiplier (on Options tab)</source>
        <translation type="unfinished">Questa formula non usa il moltiplicatore di costante a livello globale (su scheda Opzioni)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="45"/>
        <source>Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Mag  T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="87"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="120"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="334"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="389"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="563"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;In this formula the c.y and c.z components of &amp;quot;CVector3 c&amp;quot; are swapped before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Pine T&amp;ree  addCpixel ConstantMultiplier  ( c.y c.z Swap)</source>
        <translation type="obsolete">Pino Moltiplicatore Costante addCpixel (Scambia c.y c.z)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="608"/>
        <source>Start PT at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="628"/>
        <source>Stop PT at iteration:</source>
        <translation type="unfinished">Ferma PT all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="648"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="691"/>
        <source>cx:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="806"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Mag Transform &amp;T1   Rotation</source>
        <translation type="obsolete">Trasformazione Mag Rotazione &amp;T1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="566"/>
        <source>Pine T&amp;ree addCpixel ConstantMultiplier ( c.y c.z Swap)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="818"/>
        <source>Mag Transform &amp;T1 Rotation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1012"/>
        <source>Try both Linear and Logarithmic DelatDE distance estimation functions, to find which works best.</source>
        <translation type="unfinished">Provare entrambe le funzioni DE, Lineare Logaritmica e DeltaDE per trovare quale funziona meglio.</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1038"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1072"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1106"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1140"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1181"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1782"/>
        <source>T1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1043"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1077"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1111"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1145"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1186"/>
        <source>T1mod</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1048"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1082"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1116"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1150"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1191"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2157"/>
        <source>T2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1053"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1087"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1121"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1155"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1196"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2685"/>
        <source>T3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1058"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1092"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1126"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1160"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1201"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1740"/>
        <source>T4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1063"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1097"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1131"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1165"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1206"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1497"/>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1775"/>
        <source>T5b</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1173"/>
        <source>Slot 4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1214"/>
        <source>Transform:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1231"/>
        <source>Slot 3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1238"/>
        <source>Slot 5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1245"/>
        <source>Slot 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1265"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1376"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1383"/>
        <source>Enable slot sequence:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1390"/>
        <source>Slot 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1403"/>
        <source>Slot , Mag Transform Type and Iteration controls for additional transforms</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1459"/>
        <source>Offset z.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1845"/>
        <source>Scale z.y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2033"/>
        <source>Offset z.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2081"/>
        <source>Scale z.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2147"/>
        <source>Scale z.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2220"/>
        <source>Offset z.y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Slot , Mag Transform Type and  Iteration controls for additional transforms</source>
        <translation type="obsolete">Slot, Tipo Trasformazione Mag e controllo Iterazioni per trasformazioni addizionali</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1419"/>
        <source>Mag Transform Parameters.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1504"/>
        <source>power B.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1514"/>
        <source>power B.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1524"/>
        <source>power B.y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1534"/>
        <source>power A.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Scale  z.y</source>
        <translation type="obsolete">Scala z.y </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="1939"/>
        <source>power A.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2071"/>
        <source>power A.y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Scale  z.z</source>
        <translation type="obsolete">Scala z.z</translation>
    </message>
    <message>
        <source>Scale  z.x</source>
        <translation type="obsolete">Scala z.x</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2650"/>
        <source>T1Mod</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_mag_transforms.ui" line="2710"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>benesi_pine_tree</name>
    <message>
        <location filename="../qt_data/fractal_benesi_pine_tree.ui" line="41"/>
        <source>Pine Tree addCpixel ConstantMultiplier ( c.y c.z Swap)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pine_tree.ui" line="48"/>
        <source>This formula does not use the global addCpixel constant multiplier (on Options tab)</source>
        <translation type="unfinished">Questa formula non usa il moltiplicatore di costante a livello globale (su scheda Opzioni)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pine_tree.ui" line="231"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pine_tree.ui" line="247"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>benesi_pwr2s</name>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="35"/>
        <source>This formula does not use the global addCpixel constant multiplier (on Options tab)</source>
        <translation type="unfinished">Questa formula non usa il moltiplicatore di costante a livello globale (su scheda Opzioni)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="45"/>
        <source>Pr&amp;ism Shape</source>
        <translation type="unfinished">Forma a Pr&amp;isma</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="87"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1760"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="2041"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="126"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1727"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="2008"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="133"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="310"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="368"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="456"/>
        <source>Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Mag  T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="498"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="531"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="745"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="800"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="857"/>
        <source>Mag Transform &amp;T1 Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1168"/>
        <source>Benesi&amp; Pwr2 (Original) addCpixel ConstantMultiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1423"/>
        <source>Pine T&amp;ree Pwr2 addCpixel ConstantMultiplier ( c.y c.z Swap)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1165"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1420"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1682"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;In this formula the c.y and c.z components of &amp;quot;CVector3 c&amp;quot; are swapped before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1210"/>
        <source>Stop Pwr2 at iteration:</source>
        <translation type="unfinished">Ferma Pwr2 all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1243"/>
        <source>Start Pwr2 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1257"/>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1519"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Pine T&amp;ree  Pwr2      addCpixel ConstantMultiplier  ( c.y c.z Swap)</source>
        <translation type="obsolete">Pino Pwr2      addCpixel ConstantMultiplier  (Scambia c.y c.z)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1465"/>
        <source>Stop PT at iteration:</source>
        <translation type="unfinished">Ferma PT all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1498"/>
        <source>Start PT at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1507"/>
        <source>alternate c.y c.z swap</source>
        <translation type="unfinished">Alterna scambio c.y c.z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1685"/>
        <source>In&amp;verse_Cylindrical</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1802"/>
        <source>z.y Multiplier::</source>
        <translation type="unfinished">Moltiplicatore z.y  </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1809"/>
        <source>z scale:</source>
        <translation type="unfinished">scala z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1816"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="1966"/>
        <source>Post Off&amp;set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="2103"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_benesi_pwr2s.ui" line="2265"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>boxfold_bulbpow2</name>
    <message>
        <location filename="../qt_data/fractal_boxfold_bulbpow2.ui" line="65"/>
        <source>Z scale:</source>
        <translation type="unfinished">Scala z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_boxfold_bulbpow2.ui" line="147"/>
        <source>Box fold:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_boxfold_bulbpow2.ui" line="156"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>bristorbrot</name>
    <message>
        <location filename="../qt_data/fractal_bristorbrot.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_bristorbrot.ui" line="42"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>bristorbrot4D</name>
    <message>
        <location filename="../qt_data/fractal_bristorbrot4D.ui" line="35"/>
        <source>Offsets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_bristorbrot4D.ui" line="291"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>buffalo</name>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="41"/>
        <location filename="../qt_data/fractal_buffalo.ui" line="67"/>
        <source>PreAbs(x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="54"/>
        <source>PreAbs(y)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="80"/>
        <source>Abs(x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="93"/>
        <source>Abs(y)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="106"/>
        <source>Abs(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="119"/>
        <source>Positive Z component</source>
        <translation type="unfinished">Componente Z positivo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_buffalo.ui" line="126"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>collatz</name>
    <message>
        <location filename="../qt_data/fractal_collatz.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>collatz_mod</name>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="116"/>
        <source>Analytic Linear DE adjustment factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="184"/>
        <source>Final Scale:</source>
        <translation type="unfinished">Scala Finale:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="339"/>
        <source>Scale A:</source>
        <translation type="unfinished">Scala A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="406"/>
        <source>Vector B:</source>
        <translation type="unfinished">Vettore B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="413"/>
        <source>Scale B:</source>
        <translation type="unfinished">Scala B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="427"/>
        <source>PI Scale:</source>
        <translation type="unfinished">Scala PI:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="467"/>
        <source>PART B</source>
        <translation type="unfinished">PARTE B</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="474"/>
        <source>PART A</source>
        <translation type="unfinished">PARTE A</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="576"/>
        <source>Vector A:</source>
        <translation type="unfinished">Vettore A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="668"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="671"/>
        <source>addCpi&amp;xel c.x c.z Swap ConstantMultiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="845"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_collatz_mod.ui" line="892"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>eiffie_msltoe</name>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="161"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="168"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="393"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="400"/>
        <source>Offset. Use in normal mode or use it to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per mettere a punto le configurazioni dei parametri relativi alla modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="429"/>
        <source>Y scale:</source>
        <translation type="unfinished">Scala y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="441"/>
        <source>A&amp;nalytic Logrithmic DE adjustment</source>
        <translation type="unfinished">Modifica DE A&amp;naltico logaritmica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="498"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="523"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="526"/>
        <source>addCpixel Symmetrical Constant Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="756"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_eiffie_msltoe.ui" line="768"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>fold_box_mod1</name>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="41"/>
        <source>Fold Box Mod1 z = fabs(z + Const.F1) - fabs(z - Const.F2) - (z * Const.F3)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="66"/>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="714"/>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1032"/>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1302"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="86"/>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="999"/>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1335"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="378"/>
        <source>fabs(z - Const.F2)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="414"/>
        <source>fabs(z + Const.F1)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="447"/>
        <source>Const.F3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="666"/>
        <source>&amp;Box Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="721"/>
        <source>Stop at Iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="761"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="903"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="910"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="951"/>
        <source>Spherical Fold;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1046"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1053"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1178"/>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1235"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1276"/>
        <source>&amp;Rotation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_fold_box_mod1.ui" line="1520"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>generalized_fold_box</name>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="35"/>
        <source>Main parameters</source>
        <translation type="unfinished">Parametri principali</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="68"/>
        <source>Tetrahedron</source>
        <translation type="unfinished">Tetraedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="73"/>
        <source>Cube</source>
        <translation type="unfinished">Cubo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="78"/>
        <source>Octahedron</source>
        <translation type="unfinished">Ottaedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="83"/>
        <source>Dodecahedron</source>
        <translation type="unfinished">Dodecaedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="88"/>
        <source>OctCube</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="93"/>
        <source>Icosahedron</source>
        <translation type="unfinished">Icosaedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="98"/>
        <source>Box6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="103"/>
        <source>Box5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="139"/>
        <source>Spherical folding
fixed radius:</source>
        <translation type="unfinished">Raggio fisso
Folding sferico: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="147"/>
        <source>Spherical folding
min radius:</source>
        <translation type="unfinished">Raggio minimo 
Folding sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="243"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="303"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="487"/>
        <source>Solid:</source>
        <translation type="unfinished">Solido:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="494"/>
        <source>Box type:</source>
        <translation type="unfinished">Tipo Box:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="501"/>
        <source>Melt:</source>
        <translation type="unfinished">Sciogli:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="607"/>
        <source>&amp;Main rotation</source>
        <translation type="unfinished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="794"/>
        <source>Coloring parameters</source>
        <translation type="unfinished">Parametri colorazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="820"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="827"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="934"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="941"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="1020"/>
        <source>Absolute value of z:</source>
        <translation type="unfinished">Valore assoluto di z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="1055"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_generalized_fold_box.ui" line="1167"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>hypercomplex</name>
    <message>
        <location filename="../qt_data/fractal_hypercomplex.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_hypercomplex.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>ides</name>
    <message>
        <location filename="../qt_data/fractal_ides.ui" line="93"/>
        <source>Multiplier</source>
        <translation type="unfinished">Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ides.ui" line="264"/>
        <location filename="../qt_data/fractal_ides.ui" line="271"/>
        <source>Sub Multiplier</source>
        <translation type="unfinished">Sub Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ides.ui" line="368"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>ides2</name>
    <message>
        <location filename="../qt_data/fractal_ides2.ui" line="47"/>
        <source>Multiplier</source>
        <translation type="unfinished">Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ides2.ui" line="225"/>
        <location filename="../qt_data/fractal_ides2.ui" line="232"/>
        <source>Sub Multiplier</source>
        <translation type="unfinished">Sub Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ides2.ui" line="368"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>ifs</name>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="35"/>
        <source>Presets</source>
        <translation type="unfinished">Preset</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="79"/>
        <source>Dodecahedron</source>
        <translation type="unfinished">Dodecaedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="92"/>
        <source>Icosahedron</source>
        <translation type="unfinished">Icosaedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="105"/>
        <source>Octahedron</source>
        <translation type="unfinished">Ottaedro</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="118"/>
        <source>Menger Sponge</source>
        <translation type="unfinished">Spugna di Menger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="131"/>
        <source>Reset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="143"/>
        <source>Main parameters</source>
        <translation type="unfinished">Parametri principali</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="175"/>
        <source>abs (x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="188"/>
        <source>abs (z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="223"/>
        <source>vector offset:</source>
        <translation type="unfinished">offset vettori:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="334"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="369"/>
        <source>abs (y)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="476"/>
        <source>Menger Sponge mode</source>
        <translation type="unfinished">Modo spugna di Menger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="488"/>
        <source>&amp;Main rotation</source>
        <translation type="unfinished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="675"/>
        <source>&amp;Box folding</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="886"/>
        <source>edge:</source>
        <translation type="unfinished">margine:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="914"/>
        <source>Vectors</source>
        <translation type="unfinished">Vettori</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="1169"/>
        <source>#4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="1631"/>
        <source>#3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="1852"/>
        <location filename="../qt_data/fractal_ifs.ui" line="2591"/>
        <location filename="../qt_data/fractal_ifs.ui" line="2798"/>
        <location filename="../qt_data/fractal_ifs.ui" line="3192"/>
        <location filename="../qt_data/fractal_ifs.ui" line="3366"/>
        <location filename="../qt_data/fractal_ifs.ui" line="4853"/>
        <location filename="../qt_data/fractal_ifs.ui" line="4885"/>
        <location filename="../qt_data/fractal_ifs.ui" line="4943"/>
        <location filename="../qt_data/fractal_ifs.ui" line="5213"/>
        <source>intens.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="1928"/>
        <source>direction:</source>
        <translation type="unfinished">direzione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="2369"/>
        <source>#1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="2859"/>
        <location filename="../qt_data/fractal_ifs.ui" line="2986"/>
        <location filename="../qt_data/fractal_ifs.ui" line="3398"/>
        <location filename="../qt_data/fractal_ifs.ui" line="3804"/>
        <location filename="../qt_data/fractal_ifs.ui" line="4933"/>
        <location filename="../qt_data/fractal_ifs.ui" line="4963"/>
        <location filename="../qt_data/fractal_ifs.ui" line="5143"/>
        <location filename="../qt_data/fractal_ifs.ui" line="5268"/>
        <location filename="../qt_data/fractal_ifs.ui" line="5479"/>
        <source>dist.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="2885"/>
        <source>#9</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="3053"/>
        <source>#7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="3762"/>
        <source>Rotation</source>
        <translation type="unfinished">Rotazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="3938"/>
        <source>#8</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="3973"/>
        <source>#6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="4120"/>
        <source>enable:</source>
        <translation type="unfinished">abilita:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="4448"/>
        <source>#2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="5086"/>
        <source>#5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_ifs.ui" line="5555"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>imagsca_power_2</name>
    <message>
        <location filename="../qt_data/fractal_imagsca_power_2.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_imagsca_power_2.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>iq_bulb</name>
    <message>
        <location filename="../qt_data/fractal_iq_bulb.ui" line="40"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_iq_bulb.ui" line="47"/>
        <source>zAngle Power:</source>
        <translation type="unfinished">Angolo z Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_iq_bulb.ui" line="150"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>kalisets1</name>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="41"/>
        <source>Kalisets1 Mod</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="123"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="130"/>
        <source>KaliDucks minR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Enable Limits (box) on Rendering Engine tab,  and change bottom parameter   z: 10   to    z: 0,  and try low bailout  of 1 or 2.</source>
        <translation type="obsolete">Abilita Limiti (box) sulla scheda Motore di rendering, e cambiare i parametri in fondo,  z: 10  a  z: 0, provare anche valori bassi di bailout,  1 o 2.</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="212"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="199"/>
        <source>Enable Limits (box) on Rendering Engine tab, and change bottom parameter z: 10 to z: 0, and try low bailout of 1 or 2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="215"/>
        <source>Sing&amp;le Formula addCpixel Constant Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="332"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="436"/>
        <source>Single Formula Julia &amp;mode</source>
        <translation type="unfinished">&amp;Modo Julia e Formula Singola</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="543"/>
        <source>Julia constant</source>
        <translation type="unfinished">Costante di Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="555"/>
        <source>Internal &amp;Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_kalisets1.ui" line="742"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>lkmitch</name>
    <message>
        <location filename="../qt_data/fractal_lkmitch.ui" line="32"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_lkmitch.ui" line="39"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>makin3d2</name>
    <message>
        <location filename="../qt_data/fractal_makin3d2.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_makin3d2.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbox</name>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="35"/>
        <source>Main parameters</source>
        <translation type="unfinished">Parametri principali</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="61"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="96"/>
        <source>Spherical folding
fixed radius:</source>
        <translation type="unfinished">Raggio fisso
Folding sferico: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="136"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="199"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="318"/>
        <source>Spherical folding
min radius:</source>
        <translation type="unfinished">Raggio minimo 
Folding sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="326"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="559"/>
        <source>&amp;Main rotation</source>
        <translation type="unfinished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="746"/>
        <source>&amp;Rotation of separate planes</source>
        <translation type="unfinished">&amp;Rotazione piani separati</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="770"/>
        <source>X Axis</source>
        <translation type="unfinished">Asse X</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1108"/>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1240"/>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1620"/>
        <source>Positive plane</source>
        <translation type="unfinished">Piano positivo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1118"/>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1250"/>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1630"/>
        <source>Negative plane</source>
        <translation type="unfinished">Piano negativo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1140"/>
        <source>Y Axis</source>
        <translation type="unfinished">Asse Y</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1510"/>
        <source>Z Axis</source>
        <translation type="unfinished">Asse Z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1883"/>
        <source>Coloring parameters</source>
        <translation type="unfinished">Parametri colorazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1909"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="1916"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="2023"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="2030"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="2109"/>
        <source>Absolute value of z:</source>
        <translation type="unfinished">Valore assoluto di z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="2144"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox.ui" line="2256"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>mandelbox_menger</name>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="68"/>
        <source>Stop at Iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="94"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="310"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1082"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2932"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="107"/>
        <source>Box Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="121"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="156"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="207"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="317"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1075"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2964"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="376"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="478"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="485"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="698"/>
        <source>Spherical Fold;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="720"/>
        <source>&amp;Main rotation</source>
        <translation type="unfinished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="771"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="803"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1094"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1159"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1181"/>
        <source>&amp;Rotation of separate folding planes</source>
        <translation type="unfinished">&amp;Rotazione piani di folding separati</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1591"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1799"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2132"/>
        <source>Positive plane</source>
        <translation type="unfinished">Piano positivo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1607"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1666"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2447"/>
        <source>Negative plane</source>
        <translation type="unfinished">Piano negativo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="1639"/>
        <source>X Axis</source>
        <translation type="unfinished">Asse X</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2089"/>
        <source>Y Axis</source>
        <translation type="unfinished">Asse Y</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2434"/>
        <source>Z Axis</source>
        <translation type="unfinished">Asse Z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2551"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3578"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2554"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier for Mandelbox iterations</source>
        <translation type="unfinished">addCpi&amp;xel Scambio Assi ConstantMultiplier per iterazioni Mandelbox</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2583"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2649"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3806"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2761"/>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3875"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2849"/>
        <source>Men&amp;ger_Sponge</source>
        <translation type="unfinished">Spugna_di_Men&amp;ger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="2919"/>
        <source>Menger sub-loop Iterations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3135"/>
        <source>Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3471"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3518"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3531"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3581"/>
        <source>addCpixe&amp;l Axis Swap ConstantMultiplier for Menger Sponge iterations</source>
        <translation type="unfinished">addCpixe&amp;l Scambio Assi ConstantMultiplier per iterazioni Spugna Menger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3943"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3970"/>
        <source>Fold component</source>
        <translation type="unfinished">Componente Folding</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="3977"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4012"/>
        <source>Fold function components -group weight:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4106"/>
        <source>Scale component</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4523"/>
        <source>Absolute value of z component</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Scale  component</source>
        <translation type="obsolete">Componente Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4113"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4170"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4199"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4240"/>
        <source>Initial Color Min:</source>
        <translation type="unfinished">Colore Min iniziale:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4510"/>
        <source>Coloring components (beta version only)</source>
        <translation type="unfinished">Componenti colorazione (solo versione beta)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4536"/>
        <source>Orbit trap component</source>
        <translation type="unfinished">Componente orbit trap</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_menger.ui" line="4545"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>mandelbox_vary_scale_4d</name>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="362"/>
        <source>Box fold size:</source>
        <translation type="unfinished">Dimensioni Folding Box:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="242"/>
        <source>Vary scale:</source>
        <translation type="unfinished">Varia Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="299"/>
        <source>R power:</source>
        <translation type="unfinished">Potenza R:</translation>
    </message>
    <message>
        <source>w-axis constant:</source>
        <translation type="obsolete">Costante asse -w:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="513"/>
        <source>Spherical fold size:</source>
        <translation type="unfinished">Dimensioni Folding sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="520"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="939"/>
        <source>Coloring parameters</source>
        <translation type="unfinished">Parametri colorazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="965"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="1050"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="1271"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="1278"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="1207"/>
        <source>Absolute value of z:</source>
        <translation type="unfinished">Valore assoluto di z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="193"/>
        <source>Parabolic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="200"/>
        <source>Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="207"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="214"/>
        <source>z.w parabolic = paraOffset + iter *slope + (iter *iter *scale)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="221"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="532"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="574"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="581"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="1242"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbox_vary_scale_4d.ui" line="1312"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>mandelbulb</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb.ui" line="68"/>
        <source>Alpha angle offset:</source>
        <translation type="unfinished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb.ui" line="150"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb.ui" line="157"/>
        <source>Beta angle offset:</source>
        <translation type="unfinished">Offset angolo Beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb.ui" line="192"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb2</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb2.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb2.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb3</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb3.ui" line="32"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb3.ui" line="39"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>mandelbulb4</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb4.ui" line="118"/>
        <source>Alpha angle offset:</source>
        <translation type="unfinished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb4.ui" line="200"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb4.ui" line="207"/>
        <source>Beta angle offset:</source>
        <translation type="unfinished">Offset angolo Beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb4.ui" line="214"/>
        <source>Gamma angle offset:</source>
        <translation type="unfinished">Offset angolo Gamma:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb4.ui" line="276"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>mandelbulb_bermarte</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="110"/>
        <source>Rotate about z axis:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="145"/>
        <source>Beta angle offset1:</source>
        <translation type="unfinished">offset2 angolo beta: {1:?}</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="152"/>
        <source>Bulb Power:</source>
        <translation type="unfinished">Potenza bulb:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="212"/>
        <source>Beta angle offset2:</source>
        <translation type="unfinished">offset2 angolo beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="219"/>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="474"/>
        <source>Enable cos-tan mode (disable sin-tan2 mode)</source>
        <translation type="unfinished">abilita modo cos-tan  (disabilita modo sin-tan2)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="328"/>
        <source>Phi Scale:</source>
        <translation type="unfinished">Scala Phi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="335"/>
        <source>Theta Scale:</source>
        <translation type="unfinished">Scala Teta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="524"/>
        <source>Analytic &amp;Logrithmic DE adjustment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="553"/>
        <source>Analytic Logrithmic DE offset1:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="560"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="664"/>
        <source>Analytic Logrithmic DE offset2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_bermarte.ui" line="676"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb_kali</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="38"/>
        <source>fa&amp;bs() Controls</source>
        <translation type="unfinished">Controlli fa&amp;bs()</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="183"/>
        <source>Bulb Power:</source>
        <translation type="unfinished">Potenza bulb:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="190"/>
        <source>Theta Scale:</source>
        <translation type="unfinished">Scala Teta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="285"/>
        <source>Rotate about z axis:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="336"/>
        <source>Beta angle offset2:</source>
        <translation type="unfinished">offset2 angolo beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="406"/>
        <source>Beta angle offset1:</source>
        <translation type="unfinished">offset2 angolo beta: {1:?}</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="457"/>
        <source>Phi Scale:</source>
        <translation type="unfinished">Scala Phi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="469"/>
        <source>Analytic &amp;Logrithmic DE adjustment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="498"/>
        <source>Analytic Logrithmic DE offset1:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="505"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="609"/>
        <source>Analytic Logrithmic DE offset2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali.ui" line="621"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb_kali_multi</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="38"/>
        <source>fa&amp;bs() Controls</source>
        <translation type="unfinished">Controlli fa&amp;bs()</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="120"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="365"/>
        <source>Enable sine mode (disable cosine mode)</source>
        <translation type="unfinished">Abilita modo seno (disabilita modo coseno)</translation>
    </message>
    <message>
        <source>Polar to Cartesian -  Cosine Mode z.z = cos(th);</source>
        <translation type="obsolete">Da Polare a Cartesiano - Modo coseno z.z = cos(th);</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="136"/>
        <source>Rotate about z axis:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="168"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="756"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Enable seperate &amp;trig control of second  function</source>
        <translation type="obsolete">Sbilita controllo &amp;trig separato della seconda funzione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="200"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="710"/>
        <source>function order of x, y and z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="211"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="697"/>
        <source>acos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="216"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="702"/>
        <source>asin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="259"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="721"/>
        <source>atan</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="264"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="726"/>
        <source>atan2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="272"/>
        <source>ph0 function2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="282"/>
        <source>th0 function2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="543"/>
        <source>Beta angle offset2:</source>
        <translation type="unfinished">offset2 angolo beta:</translation>
    </message>
    <message>
        <source>Phi  Scale:</source>
        <translation type="obsolete">Scala Phi: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="127"/>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="372"/>
        <source>Polar to Cartesian - Cosine Mode z.z = cos(th);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="171"/>
        <source>Enable seperate &amp;trig control of second function</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="557"/>
        <source>Phi Scale:</source>
        <translation type="unfinished">Scala Phi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="583"/>
        <source>Bulb Power:</source>
        <translation type="unfinished">Potenza bulb:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="590"/>
        <source>Beta angle offset1:</source>
        <translation type="unfinished">offset2 angolo beta: {1:?}</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="622"/>
        <source>Theta Scale:</source>
        <translation type="unfinished">Scala Teta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="734"/>
        <source>th0 function:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="744"/>
        <source>ph0 function:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="759"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="961"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1005"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1019"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1034"/>
        <source>Analytic &amp;Logrithmic DE adjustment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1063"/>
        <source>Analytic Logrithmic DE offset1:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1070"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1174"/>
        <source>Analytic Logrithmic DE offset2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_kali_multi.ui" line="1186"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb_multi</name>
    <message>
        <source>Mandelbulb  Multi_Trig Formula;</source>
        <translation type="obsolete">Formula Mandelbulb  Multi_Trig</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="35"/>
        <source>Mandelbulb Multi_Trig Formula;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="62"/>
        <source>fabs() Controls (&amp;Burning Ship mode)</source>
        <translation type="unfinished">Controlli fabs() (modo &amp;Burning Ship)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="132"/>
        <source>function order of x, y and z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="143"/>
        <source>atan</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="148"/>
        <source>atan2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="156"/>
        <source>th0 function:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="166"/>
        <source>ph0 function:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="211"/>
        <source>acos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="216"/>
        <source>asin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="294"/>
        <source>Phi scale:</source>
        <translation type="unfinished">Scala Phi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="326"/>
        <source>Beta angle offset:</source>
        <translation type="unfinished">Offset angolo Beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="464"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="471"/>
        <source>Theta scale:</source>
        <translation type="unfinished">Scala Teta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="497"/>
        <source>Alpha angle offset:</source>
        <translation type="unfinished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="539"/>
        <source>Enable cosine mode (disable sine mode)</source>
        <translation type="unfinished">Abilita modo coseno (disabilita modo seno)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="546"/>
        <source>Polar to Cartesian - Sine Mode z.z = sin(th);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="560"/>
        <source>in Sine mode, enable costh = cos(th0)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="567"/>
        <source>in Cosine mode, enable sinth = sin(th0)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Polar to Cartesian -  Sine Mode z.z = sin(th);</source>
        <translation type="obsolete">Da polare a Cartesiano - Modo Seno z.z = sin(th);</translation>
    </message>
    <message>
        <source>in Sine mode, enable   costh = cos(th0)</source>
        <translation type="obsolete">in modo Seno, abilita  costh = cos(th0)</translation>
    </message>
    <message>
        <source>in Cosine mode, enable  sinth = sin(th0)</source>
        <translation type="obsolete">In modo Coseno, abilita sinth = sin(th0)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="586"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="589"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="825"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="842"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="849"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_multi.ui" line="861"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb_power_2</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_power_2.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_power_2.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mandelbulb_vary_powerV1</name>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="132"/>
        <source>Power Variable:</source>
        <translation type="unfinished">Variabile Potenza:</translation>
    </message>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="obsolete">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="139"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="146"/>
        <source>Alpha angle offset:</source>
        <translation type="unfinished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="153"/>
        <source>Beta angle offset:</source>
        <translation type="unfinished">Offset angolo Beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="160"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="167"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mandelbulb_vary_powerV1.ui" line="336"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>menger_4D</name>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="188"/>
        <source>PreAdd Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="195"/>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1144"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="202"/>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1124"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="396"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="403"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="354"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="766"/>
        <source>4D Menger Sponge</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="784"/>
        <source>Menger Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="907"/>
        <source>Menger offsets:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1095"/>
        <source>S&amp;pherical Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1237"/>
        <source>Maximum Radius2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1244"/>
        <source>Minimum Radius2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1316"/>
        <source>I&amp;teration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1540"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1345"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1352"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1359"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1438"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1445"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1583"/>
        <source>DE tweak temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D.ui" line="1645"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_4D_mod1</name>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="193"/>
        <source>Parabolic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="200"/>
        <source>Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="207"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="221"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="383"/>
        <source>PreAdd Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="390"/>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1409"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="397"/>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1389"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="591"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="598"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="214"/>
        <source>z.w parabolic = paraOffset + iter *slope + (iter *iter *scale)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="549"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="961"/>
        <source>4D Menger Sponge Mod1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="980"/>
        <source>Start cond.z at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1000"/>
        <source>Stop cond.z at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1023"/>
        <source>conditional z.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1049"/>
        <source>Menger Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1172"/>
        <source>Menger offsets:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1360"/>
        <source>S&amp;pherical Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1446"/>
        <source>Dot2 mode (x*x+y*y);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1453"/>
        <source>Dot3 mode (x*x+y*y+z*z);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1460"/>
        <source>Dot4 mode (x*x+y*y+z*z+w*w);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1606"/>
        <source>I&amp;teration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1830"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1527"/>
        <source>Maximum Radius2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1534"/>
        <source>Minimum Radius2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1635"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1642"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1649"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1728"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1735"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1873"/>
        <source>DE tweak temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_4D_mod1.ui" line="1935"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_cross_kifs</name>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="38"/>
        <source>Prism S&amp;hape Mod2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="99"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="295"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="401"/>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1349"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="421"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="428"/>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1310"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="454"/>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1356"/>
        <source>enable fabs(z.x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="604"/>
        <source>Post Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Apply  z.x condition</source>
        <translation type="obsolete">Applica condizione z.x</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="634"/>
        <source>Apply z.x condition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="789"/>
        <source>z.y scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="852"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="909"/>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1816"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1118"/>
        <source>Prism Shape Rotation</source>
        <translation type="unfinished">Rotazione Forma Prismatica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1136"/>
        <source>DE tweak:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1228"/>
        <source>Cross Menger KIFS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1239"/>
        <source>enable CrossMengerKIFS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1391"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1400"/>
        <source>enable Fold Modes 1 and 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1409"/>
        <source>z.x Mode 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1416"/>
        <source>z.z Mode 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1423"/>
        <source>z.y Mode 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1621"/>
        <source>Fold:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1851"/>
        <source>z.x offset B</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1911"/>
        <source>z.x offset A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1947"/>
        <source>enable fabs(z.x) Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="2005"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="1963"/>
        <source>&amp;Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="2038"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_kifs.ui" line="2217"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>menger_cross_mod1</name>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="41"/>
        <source>Prism Shape Mod1</source>
        <translation type="unfinished">Mod1 Forma Prismatica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="53"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="249"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="342"/>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="847"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="349"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="369"/>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="955"/>
        <source>enable fabs(z.x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="376"/>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="889"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="428"/>
        <source>enable Prism Shape</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="446"/>
        <source>Apply z.x condition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="831"/>
        <source>enable Mod1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="982"/>
        <source>enable fabs(z.x) Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="1167"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <source>Apply  z.x condition</source>
        <translation type="obsolete">Applica condizione z.x</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="568"/>
        <source>z.y scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="575"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="742"/>
        <source>Prism Shape Rotation</source>
        <translation type="unfinished">Rotazione Forma Prismatica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="760"/>
        <source>DE tweak:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="838"/>
        <source>enable CrossMengerTrick</source>
        <translation type="unfinished">abilita CrossMengerTrick  </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="882"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="968"/>
        <source>CrossMengerTrick Mod1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="1042"/>
        <source>z.x offset A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="1049"/>
        <source>z.x offset B</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="1125"/>
        <source>&amp;Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="1200"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_cross_mod1.ui" line="1379"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>menger_middle_mod</name>
    <message>
        <source>Menger Sponge Start Function     Fabs function z = fabs( z + const.A);</source>
        <translation type="obsolete">Inizia Funzione Spugna di Menger      Funzione fabs di z = fabs( z + const.A);</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="35"/>
        <source>Menger Sponge Start Function Fabs function z = fabs( z + const.A);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="141"/>
        <source>Const.A x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="173"/>
        <source>Const.A y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="180"/>
        <source>Const.A z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="224"/>
        <source>Menger Sponge Middle Functions</source>
        <translation type="unfinished">Funzioni intermedie Spugna di Menger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="231"/>
        <source>Middle &amp;Rotation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="257"/>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="523"/>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="802"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="290"/>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="822"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="475"/>
        <source>Middle &amp;Box Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="530"/>
        <source>Stop at Iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="570"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="712"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="719"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="760"/>
        <source>Middle FabsBoxFold (z = fabs(z + Const.F1) - fabs(z - Const.F2) - (z * Const.F3)) * Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1543"/>
        <source>Menger Sponge End Function Apply Constants;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1797"/>
        <source>Conditional mode: if ( z.z &lt; limit ) z.z = z.z;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Middle FabsBoxFold (z = fabs(z  + Const.F1)  -  fabs(z - Const.F2)  -  (z * Const.F3)) * Scale</source>
        <translation type="obsolete">FabsBoxFold Intermedio (z = fabs(z  + Const.F1)  -  fabs(z - Const.F2)  -  (z * Const.F3)) * Scale</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="873"/>
        <source>Start scale at iteration:</source>
        <translation type="unfinished">Inizia Scala all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="880"/>
        <source>Stop scale at iteration:</source>
        <translation type="unfinished">Ferma scala all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1319"/>
        <source>fabs(z - Const.F2)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1326"/>
        <source>fabs(z + Const.F1)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1362"/>
        <source>Const.F3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1515"/>
        <source>FabsBoxFold Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1555"/>
        <source>Offset Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1562"/>
        <source>Menger Sponge scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1806"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="1809"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="2045"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="2055"/>
        <source>Cpixel Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_middle_mod.ui" line="2074"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>menger_mod1</name>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="205"/>
        <source>Internal &amp;Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="247"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="280"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="452"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="494"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="593"/>
        <source>Multiplier</source>
        <translation type="unfinished">Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="723"/>
        <source>&amp;Box Offset</source>
        <translation type="unfinished">Offset &amp;Box</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="765"/>
        <source>Stop Offset at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1021"/>
        <source>I&amp;teration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1245"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Offset at iteration:</source>
        <translation type="obsolete">Ferma Offset all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="798"/>
        <source>Start Offset at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="957"/>
        <source>Box Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="999"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Can sometimes improve render quality when using analytic DE&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A volte può migliorare la qualità di renering usando la DE analitica&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1002"/>
        <source>Analytic DE calculation</source>
        <translation type="unfinished">Calcolo DE analitica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1050"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1057"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1064"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1143"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1150"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_mod1.ui" line="1286"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_octo</name>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="35"/>
        <source>Octo</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="60"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="687"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1983"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="93"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="720"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2016"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="160"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="224"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2251"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="361"/>
        <source>&amp;Box Offset</source>
        <translation type="unfinished">Offset &amp;Box</translation>
    </message>
    <message>
        <source>Stop  Offset at iteration:</source>
        <translation type="obsolete">Ferma Offset all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="403"/>
        <source>Stop Offset at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="436"/>
        <source>Start Offset at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="595"/>
        <source>Box Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="637"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="659"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Can sometimes improve render quality when using analytic DE&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A volte può migliorare la qualità di renering usando la DE analitica&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="640"/>
        <source>Analytic DE calculation</source>
        <translation type="unfinished">Calcolo DE analitica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="662"/>
        <source>Spherical Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="756"/>
        <source>Initial minR2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="791"/>
        <source>Fixed maxR2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="921"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1005"/>
        <source>Post_scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1133"/>
        <source>&amp;Spherical Fold minR2 vary curvilinear (VCL)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1175"/>
        <source>Curvilinear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1182"/>
        <source>Iteration B:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1189"/>
        <source>A Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1237"/>
        <source>B Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1275"/>
        <source>Note. Curvi Function requires the condition that B &gt; 2 * A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1426"/>
        <source>Curvi funtion ( disable to use linear slopes only)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1746"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2466"/>
        <source>I&amp;teration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2690"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1348"/>
        <source>C Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1355"/>
        <source>0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1412"/>
        <source>Iteration A:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1419"/>
        <source>Iteration C:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1483"/>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1664"/>
        <source>Initial add:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1490"/>
        <source>Iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1657"/>
        <source>Parabolic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1671"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1678"/>
        <source>The Curviliner and Parabolic functions can be applied together</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1685"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1704"/>
        <source>R&amp;otation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1779"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="1958"/>
        <source>Menger Sponge</source>
        <translation type="unfinished">Spugna di Menger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2202"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2314"/>
        <source>Multiplier</source>
        <translation type="unfinished">Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2495"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2502"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2509"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2588"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2595"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_octo.ui" line="2731"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_prism_shape</name>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="40"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="209"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1804"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2729"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="216"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1784"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2684"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="229"/>
        <source>Prism Shape</source>
        <translation type="unfinished">Forma a Prisma</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="236"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="768"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1296"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="403"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="415"/>
        <source>Rotat&amp;ion</source>
        <translation type="unfinished">Rotatz&amp;ione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="462"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="469"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="686"/>
        <source>Benesi Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="728"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="761"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="998"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1526"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2545"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1037"/>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1565"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1214"/>
        <source>Benesi Mag Transform T&amp;1 Mod</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1 Mod</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1256"/>
        <source>Stop T1 Mod at iteration:</source>
        <translation type="unfinished">Ferma T1 Mod all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1289"/>
        <source>Start T1 Mod at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1742"/>
        <source>fABS Add Constant Multi z = fabs( z + const.A ) - fabs( z - const.B )&amp; - ( z * const.C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="1896"/>
        <source>- ( z * C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )&amp;  - (  z * const.C  )</source>
        <translation type="obsolete">fABS Aggiunge Multi Costantei   z = fabs(z + const.A) - fabs(z - const.B)&amp;  - (z * const.C)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2006"/>
        <source>Enable fABS(z - B)</source>
        <translation type="unfinished">Abilita fABS(z - B)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2364"/>
        <source>Enable fABS(z + A)</source>
        <translation type="unfinished">Abilita fABS(z + A)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2633"/>
        <source>Men&amp;ger Sponge</source>
        <translation type="unfinished">Spugna di Men&amp;ger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="2817"/>
        <source>Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="3047"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="3144"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="3157"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="3379"/>
        <source>Temp DE fudge factor</source>
        <translation type="unfinished">Fattore Temp fudge DE</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape.ui" line="3447"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>menger_prism_shape2</name>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="35"/>
        <source>Swizzle z = CVector3{-z.z, z.x, z.y}</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="54"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="463"/>
        <source>mode 0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="468"/>
        <source>mode 1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="473"/>
        <source>mode 2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="478"/>
        <source>mode 3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="483"/>
        <source>mode 4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="488"/>
        <source>mode 5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="493"/>
        <source>mode 6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="498"/>
        <source>mode 7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="512"/>
        <source>Prism Shape2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="519"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1816"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2344"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="529"/>
        <source>Dot modes:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="538"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1023"/>
        <source>enable fabs(z.x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="545"/>
        <source>enable fabs(z.y)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="552"/>
        <source>enable fabs(z.z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="593"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="983"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1348"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2852"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4048"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="600"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1016"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1385"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2832"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4003"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="609"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="616"/>
        <source>z.x shift:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="623"/>
        <source>z.y shift</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="644"/>
        <source>z.x &gt;= 0 conditional</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="651"/>
        <source>Apply yz floor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="742"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="749"/>
        <source>z.y scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="865"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1085"/>
        <source>DE tweak:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="904"/>
        <source>post_scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="941"/>
        <source>CrossMe&amp;ngerTrick</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1033"/>
        <source>enable mod1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1050"/>
        <source>deriv. Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1092"/>
        <source>cmt Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1219"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1405"/>
        <source>DotScale::</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1319"/>
        <source>CrossMengerKI&amp;FS2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1355"/>
        <source>cmtKIFS Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1365"/>
        <source>dd thingy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1576"/>
        <source>enable fabs(z.x) Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2790"/>
        <source>fABS Add Constant Multi z = fabs( z + const.A ) - fabs( z - const.B )&amp; - ( z * const.C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2944"/>
        <source>- ( z * C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1636"/>
        <source>z.x offset A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1643"/>
        <source>z.x offset B</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1734"/>
        <source>Benesi Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1776"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="1809"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2046"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2574"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3593"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2085"/>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2613"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2262"/>
        <source>Benesi Mag Transform T&amp;1 Mod</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1 Mod</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2304"/>
        <source>Stop T1 Mod at iteration:</source>
        <translation type="unfinished">Ferma T1 Mod all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="2337"/>
        <source>Start T1 Mod at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )&amp;  - (  z * const.C  )</source>
        <translation type="obsolete">fABS Aggiunge Multi Costantei   z = fabs(z + const.A) - fabs(z - const.B)&amp;  - (z * const.C)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3054"/>
        <source>Enable fABS(z - B)</source>
        <translation type="unfinished">Abilita fABS(z - B)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3412"/>
        <source>Enable fABS(z + A)</source>
        <translation type="unfinished">Abilita fABS(z + A)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3675"/>
        <source>Rotat&amp;ion</source>
        <translation type="unfinished">Rotatz&amp;ione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3722"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3729"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="3952"/>
        <source>Men&amp;ger Sponge</source>
        <translation type="unfinished">Spugna di Men&amp;ger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4102"/>
        <source>Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4332"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4429"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4626"/>
        <source>Temp DE fudge factor</source>
        <translation type="unfinished">Fattore Temp fudge DE</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_prism_shape2.ui" line="4694"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>menger_pwr2_poly</name>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="40"/>
        <source>Scale B:</source>
        <translation type="unfinished">Scala B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="99"/>
        <source>Stop:</source>
        <translation type="unfinished">Ferma:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="106"/>
        <source>Polynomial iterations. Start:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="120"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="161"/>
        <source>Scale A:</source>
        <translation type="unfinished">Scala A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="274"/>
        <source>Vector A:</source>
        <translation type="unfinished">Vettore A:</translation>
    </message>
    <message>
        <source>PART B =  ScaleB *  z  + VectorC</source>
        <translation type="obsolete">PARTE B =  ScaleB *  z  + VectorC</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="390"/>
        <source>fabs(z):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="397"/>
        <source>Analytic Linear DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="432"/>
        <source>fabs(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="471"/>
        <source>fn( z) = Pi Rotation about &amp;VectorA</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="528"/>
        <source>Pi Rotation:</source>
        <translation type="unfinished">Rotazione Pi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="630"/>
        <source>Power3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Power2 Polynomial   Az^2 + Bz + C</source>
        <translation type="obsolete">Power2 Polinomiale Az^2 + Bz + C</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="670"/>
        <source>Vector C:</source>
        <translation type="unfinished">Vettore C:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="684"/>
        <source>Final Scale:</source>
        <translation type="unfinished">Scala Finale:</translation>
    </message>
    <message>
        <source>PART A   =  - (ScaleA * z + VectA)  *  fn(z)</source>
        <translation type="obsolete">PARTE A   =  - (ScaleA * z + VectA)  *  fn(z)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="720"/>
        <source>fn( z) = &amp;Box Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="830"/>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="2177"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="966"/>
        <source>Analytic Linear DE offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="999"/>
        <source>fabs(fn(z)):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="329"/>
        <source>PART B = ScaleB * z + VectorC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="361"/>
        <source>fn( z) options, (default = z;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="644"/>
        <source>Power2 Polynomial Az^2 + Bz + C</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="710"/>
        <source>PART A = - (ScaleA * z + VectA) * fn(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1049"/>
        <source>fn( z) = cos ( z * pi )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1078"/>
        <source>Pi Scale:</source>
        <translation type="unfinished">Scala Pi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1198"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1201"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1396"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1447"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1461"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1476"/>
        <source>Menger_Sponge</source>
        <translation type="unfinished">Spugna_di_Menger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1527"/>
        <source>Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1584"/>
        <source>Internal &amp;Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1626"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="1659"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="2141"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="2148"/>
        <source>Menger Iterations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_pwr2_poly.ui" line="2217"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_smooth</name>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="153"/>
        <source>R&amp;otation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="228"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="457"/>
        <source>Menger Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="464"/>
        <source>OffsetC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="565"/>
        <source>OffsetS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="195"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="602"/>
        <source>I&amp;teration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="694"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="701"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="721"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="728"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="763"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="848"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth.ui" line="867"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_smooth_mod1</name>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="38"/>
        <source>Menger Smooth Mod1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="126"/>
        <source>Fabs Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="133"/>
        <source>ScaleC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="140"/>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="183"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="147"/>
        <source>OffsetS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="176"/>
        <source>Menger Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="333"/>
        <source>R&amp;otation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="375"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="714"/>
        <source>OffsetS Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1050"/>
        <source>Stop Offset at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1296"/>
        <source>I&amp;teration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1542"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0 to 1.1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Stop  Rotation at iteration:</source>
        <translation type="obsolete">Ferma Rotazione all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="408"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="622"/>
        <source>OffsetC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="839"/>
        <source>Squares Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1008"/>
        <source>&amp;Box Offset</source>
        <translation type="unfinished">Offset &amp;Box</translation>
    </message>
    <message>
        <source>Stop  Offset at iteration:</source>
        <translation type="obsolete">Ferma Offset all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1083"/>
        <source>Start Offset at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1242"/>
        <source>Box Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1388"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1395"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1415"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1422"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1457"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_smooth_mod1.ui" line="1561"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>menger_sponge</name>
    <message>
        <location filename="../qt_data/fractal_menger_sponge.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_menger_sponge.ui" line="42"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>mixPinski_4D</name>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="40"/>
        <source>Sierpinski</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="73"/>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="322"/>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="953"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="93"/>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="329"/>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="933"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="100"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="315"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="481"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="523"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="530"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="913"/>
        <source>4D Menger Sponge</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="964"/>
        <source>Menger Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="1087"/>
        <source>Menger offsets:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="1270"/>
        <source>DE tweak temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_mixPinski_4D.ui" line="1332"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>msltoe_donut</name>
    <message>
        <location filename="../qt_data/fractal_msltoe_donut.ui" line="87"/>
        <source>Ring number:</source>
        <translation type="unfinished">Numero anelli:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_donut.ui" line="132"/>
        <source>Ring radius:</source>
        <translation type="unfinished">Raggio anelli:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_donut.ui" line="195"/>
        <source>Ring thickness:</source>
        <translation type="unfinished">Spessore anelli:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_donut.ui" line="221"/>
        <source>Size factor:</source>
        <translation type="unfinished">fattore dimensioni: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_donut.ui" line="258"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>msltoe_toroidal</name>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="38"/>
        <source>P&amp;re-Scale</source>
        <translation type="unfinished">P&amp;re-Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="159"/>
        <source>Pre-Scale</source>
        <translation type="unfinished">Pre-Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="235"/>
        <source>Theta power</source>
        <translation type="unfinished">Potenza Teta</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="242"/>
        <source>Radius power</source>
        <translation type="unfinished">Potenza raggio</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="321"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="384"/>
        <source>Phi power</source>
        <translation type="unfinished">Potenza Phi</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="475"/>
        <source>Analytic &amp;Logrithmic DE adjustment ( beta version only, may need to force Delta DE method)</source>
        <translation type="unfinished">Ritocco DE Analitica &amp;Logaritmica (solo versione beta, potrebbe essere necessario usare il metodo Forza Delta DE)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="504"/>
        <source>Analytic Logrithmic DE offset1:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="511"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="615"/>
        <source>Analytic Logrithmic DE offset2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="622"/>
        <source>enable aux.r_dz squared</source>
        <translation type="unfinished">Abilita aux.r_dz al quadrato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="644"/>
        <source>Spherica&amp;l Offset</source>
        <translation type="unfinished">Offset &amp;Sferico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="695"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="762"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal.ui" line="774"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>msltoe_toroidal_multi</name>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="38"/>
        <source>P&amp;re-Scale</source>
        <translation type="unfinished">P&amp;re-Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="159"/>
        <source>Pre-Scale</source>
        <translation type="unfinished">Pre-Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="228"/>
        <source>function order of x, y and z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="239"/>
        <source>atan2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="244"/>
        <source>atan</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="252"/>
        <source>th0 function:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="262"/>
        <source>ph0 function:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="307"/>
        <source>asin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="312"/>
        <source>acos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="330"/>
        <source>sqrt(aux.r)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="372"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="407"/>
        <source>Radius power</source>
        <translation type="unfinished">Potenza raggio</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="493"/>
        <source>Beta angle offset:</source>
        <translation type="unfinished">Offset angolo Beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="528"/>
        <source>Th0 Scale</source>
        <translation type="unfinished">Scala Th0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="638"/>
        <source>Alpha angle offset:</source>
        <translation type="unfinished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="679"/>
        <source>Enable cosine mode (disable sine mode)</source>
        <translation type="unfinished">Abilita modo coseno (disabilita modo seno)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="686"/>
        <source>Polar to Cartesian - Sine Mode z.z = sin(th0);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Polar to Cartesian -  Sine Mode z.z = sin(th0);</source>
        <translation type="obsolete">Da Polare a Cartesiano -  Modo Seno z.z = sin(th0)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="720"/>
        <source>Ph0 Scale</source>
        <translation type="unfinished">Scala Ph0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="739"/>
        <source>Analytic &amp;Logrithmic DE adjustment ( beta version only, may need to force Delta DE method)</source>
        <translation type="unfinished">Ritocco DE Analitica &amp;Logaritmica (solo versione beta, potrebbe essere necessario usare il metodo Forza Delta DE)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="768"/>
        <source>Analytic Logrithmic DE offset1:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="775"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="879"/>
        <source>Analytic Logrithmic DE offset2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="886"/>
        <source>enable aux.r_dz squared</source>
        <translation type="unfinished">Abilita aux.r_dz al quadrato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="908"/>
        <source>Spherica&amp;l Offset</source>
        <translation type="unfinished">Offset &amp;Sferico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="975"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="1026"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoe_toroidal_multi.ui" line="1038"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>msltoesym2_mod</name>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="153"/>
        <source>Global Constant Multiplier of c.z must be set at 0.0 for this formula</source>
        <translation type="unfinished">Moltiplicatore Costanti globale di c.z, dev&apos;essere impostato a 0 per questa formula</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="195"/>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="227"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="266"/>
        <source>Y scale:</source>
        <translation type="unfinished">Scala y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="273"/>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="280"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="287"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="398"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="401"/>
        <source>addCpixel Symmetrical Constant &amp;Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="631"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym2_mod.ui" line="643"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>msltoesym3_mod</name>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="47"/>
        <source>addYZ Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="213"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="220"/>
        <source>Y scale:</source>
        <translation type="unfinished">Scala y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="316"/>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="465"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="472"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="488"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="491"/>
        <source>addCpixel Symmetrical Constant &amp;Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="721"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="733"/>
        <source>&amp;Quaternion Fold</source>
        <translation type="unfinished">Folding &amp;Quaternione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="762"/>
        <source>Stop at Iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="769"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="849"/>
        <source>Enable DE Vector mode</source>
        <translation type="unfinished">Abilita modo Vettori DE</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="856"/>
        <source>DE Vector Influence:</source>
        <translation type="unfinished">DE Influenza Vettore:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod.ui" line="875"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>msltoesym3_mod2</name>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="40"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="221"/>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="256"/>
        <source>addYZ Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="385"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="417"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="442"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="445"/>
        <source>addCpixel Symmetrical Constant &amp;Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="675"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod2.ui" line="687"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>msltoesym3_mod3</name>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="54"/>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="83"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="90"/>
        <source>Y scale:</source>
        <translation type="unfinished">Scala y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="182"/>
        <source>addYZ Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="470"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="477"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="502"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="505"/>
        <source>addCpixel Symmetrical Constant &amp;Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="735"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="747"/>
        <source>&amp;Quaternion Fold</source>
        <translation type="unfinished">Folding &amp;Quaternione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="776"/>
        <source>Enable DE Vector mode</source>
        <translation type="unfinished">Abilita modo Vettori DE</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="828"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="835"/>
        <source>Stop at Iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym3_mod3.ui" line="854"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>msltoesym4_mod</name>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="115"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="173"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="321"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="406"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="477"/>
        <source>Offset. Use in normal mode or use to fine tune in Julia mode</source>
        <translation type="unfinished">Offset. Da usare normalmente o per completare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="539"/>
        <source>Internal &amp;Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="726"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot; towards the origin.&lt;/p&gt;&lt;p&gt;This transform allows the addition or subtraction of Cpixel, dependent on whether z is postive or negative.&lt;/p&gt;&lt;p&gt;e.g if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is positive then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt; is added, if &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; is negative then &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; is subtracted. &lt;/p&gt;&lt;p&gt;Also try negative addCpixel constant multipliers.&lt;/p&gt;&lt;p&gt;If checkBoxes are disabled then Cpixel remains signed.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette l&apos;aggiunta o la sottrazione di Cpixel, dipendente da z, se positiva o negativa.&lt;/p&gt;&lt;p&gt;e.g se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; è positiva, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c.x)&lt;/span&gt;viene aggiunta, se &lt;span style=&quot; font-style:italic;&quot;&gt;z.x&lt;/span&gt; iè negativa, allora &lt;span style=&quot; font-style:italic;&quot;&gt;fabs(c)&lt;/span&gt; viene sottratta. &lt;/p&gt;&lt;p&gt;Provare anche ad assegnare valori negativi ai moltiplicatori di costante addCpixel.&lt;/p&gt;&lt;p&gt;Se la casella di controllo non è spuntata, allora Cpixel rimane segnata.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="729"/>
        <source>addCpixel Symmetrical Constant &amp;Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="899"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_msltoesym4_mod.ui" line="971"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>platonic_solid</name>
    <message>
        <location filename="../qt_data/fractal_platonic_solid.ui" line="40"/>
        <source>Frequency:</source>
        <translation type="unfinished">Frequenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_platonic_solid.ui" line="94"/>
        <source>Amplitude:</source>
        <translation type="unfinished">Ampiezza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_platonic_solid.ui" line="148"/>
        <source>Rho Mul:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_platonic_solid.ui" line="204"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>pseudo_kleinian1</name>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="35"/>
        <source>Note 1) : the pseudo kleinian fractal on this UI, works with the standard linear analytic DE calculation.</source>
        <translation type="unfinished">Nota 1) : il frattale pseudo kleinian in questa UI, lavora utilizzando il calcolo DE di tipo standard, analitico e lineare</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="42"/>
        <source>Note 2) : termination condition is when iteration reaches Maxiter,</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="59"/>
        <source>Pr&amp;ism Shape</source>
        <translation type="unfinished">Forma a Pr&amp;isma</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="108"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1866"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2572"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3319"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3443"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="147"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1846"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2527"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3274"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3398"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="154"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="830"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1358"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="336"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="394"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="477"/>
        <source>Rotat&amp;ion</source>
        <translation type="unfinished">Rotatz&amp;ione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="524"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="531"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="748"/>
        <source>Benesi Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="790"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="823"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1060"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1588"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1099"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1627"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1276"/>
        <source>Benesi Mag Transform T&amp;1 Mod</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1 Mod</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1318"/>
        <source>Stop T1 Mod at iteration:</source>
        <translation type="unfinished">Ferma T1 Mod all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1351"/>
        <source>Start T1 Mod at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1804"/>
        <source>fABS Add Constant Multi z = fabs( z + const.A ) - fabs( z - const.B )&amp; - ( z * const.C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="1958"/>
        <source>- ( z * C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )&amp;  - (  z * const.C  )</source>
        <translation type="obsolete">fABS Aggiunge Multi Costantei   z = fabs(z + const.A) - fabs(z - const.B)&amp;  - (z * const.C)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2186"/>
        <source>Enable fABS(z - B)</source>
        <translation type="unfinished">Abilita fABS(z - B)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2404"/>
        <source>Enable fABS(z + A)</source>
        <translation type="unfinished">Abilita fABS(z + A)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2476"/>
        <source>Men&amp;ger Sponge</source>
        <translation type="unfinished">Spugna di Men&amp;ger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2660"/>
        <source>Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2890"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="2987"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3000"/>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3956"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3223"/>
        <source>Pseu&amp;do Kleinian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3347"/>
        <source>Pseu&amp;do Kleinian Variation</source>
        <translation type="unfinished">Variante Pseu&amp;do Kleinian</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3563"/>
        <source>Pseudo Kleinian Parameters</source>
        <translation type="unfinished">Parametri Pseudo Kleinian</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3576"/>
        <source>Size R2:</source>
        <translation type="unfinished">Dimensioni R2:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3667"/>
        <source>Csize:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3834"/>
        <source>DEadd (tweak):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="3876"/>
        <source>z subtr. scale:</source>
        <translation type="unfinished">scala zubtr:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian1.ui" line="4143"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>pseudo_kleinian2</name>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="35"/>
        <source>Note 1) : termination condition is when iteration reaches Maxiter,</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="42"/>
        <source>Note 2) : each additional iteration adds more detail.</source>
        <translation type="unfinished">Nota 2): ogni iterazione addizionale incrementa il livello di dettaglio</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="49"/>
        <source>Note 3 ): this type of DE calculation will not work with other types.</source>
        <translation type="unfinished">Nota 3): questo tipo di calcolo DE non funzionerà su tipi differenti</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="66"/>
        <source>Pr&amp;ism Shape</source>
        <translation type="unfinished">Forma a Pr&amp;isma</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="108"/>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="526"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="147"/>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="519"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="154"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="295"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="453"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="477"/>
        <source>Bo&amp;x Fold</source>
        <translation type="unfinished">Folding Bo&amp;x</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="712"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="719"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="726"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="817"/>
        <source>Csize:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1009"/>
        <source>DEadd (tweak):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1097"/>
        <source>Pseudo Kleinian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1110"/>
        <source>Size R2:</source>
        <translation type="unfinished">Dimensioni R2:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1179"/>
        <source>DE factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1250"/>
        <source>Rotat&amp;ion</source>
        <translation type="unfinished">Rotatz&amp;ione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1297"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1304"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1523"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian2.ui" line="1710"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>pseudo_kleinian3</name>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="35"/>
        <source>Note 1) : termination condition is when iteration reaches Maxiter,</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="42"/>
        <source>Note 2) : each additional iteration adds more detail.</source>
        <translation type="unfinished">Nota 2): ogni iterazione addizionale incrementa il livello di dettaglio</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="49"/>
        <source>Note 3) : this type of DE calculation will not work with other types.</source>
        <translation type="unfinished">Nota 3): questo tipo di calcolo DE non funzionerà su tipi differenti</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="66"/>
        <source>Pr&amp;ism Shape</source>
        <translation type="unfinished">Forma a Pr&amp;isma</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="108"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1852"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2558"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2682"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3559"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="147"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1832"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2513"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2637"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3514"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="154"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="816"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1344"/>
        <source>Note: Default = 1</source>
        <translation type="unfinished">Nota: Default = 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="331"/>
        <source>Dot scale:</source>
        <translation type="unfinished">Prodotto scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="389"/>
        <source>Shift/offset scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="470"/>
        <source>Rotat&amp;ion</source>
        <translation type="unfinished">Rotatz&amp;ione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="512"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="519"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="734"/>
        <source>Benesi Mag Transform T&amp;1</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="776"/>
        <source>Stop T1 at iteration:</source>
        <translation type="unfinished">Ferma T1 all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="809"/>
        <source>Start T1 at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1046"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1574"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1085"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1613"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1262"/>
        <source>Benesi Mag Transform T&amp;1 Mod</source>
        <translation type="unfinished">Trasformazione Benesi Mag T&amp;1 Mod</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1304"/>
        <source>Stop T1 Mod at iteration:</source>
        <translation type="unfinished">Ferma T1 Mod all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1337"/>
        <source>Start T1 Mod at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1790"/>
        <source>fABS Add Constant Multi z = fabs( z + const.A ) - fabs( z - const.B )&amp; - ( z * const.C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="1944"/>
        <source>- ( z * C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3187"/>
        <source>z.z subtr. scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )&amp;  - (  z * const.C  )</source>
        <translation type="obsolete">fABS Aggiunge Multi Costantei   z = fabs(z + const.A) - fabs(z - const.B)&amp;  - (z * const.C)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2172"/>
        <source>Enable fABS(z - B)</source>
        <translation type="unfinished">Abilita fABS(z - B)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2390"/>
        <source>Enable fABS(z + A)</source>
        <translation type="unfinished">Abilita fABS(z + A)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2462"/>
        <source>Pseu&amp;do Kleinian</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2586"/>
        <source>Pseu&amp;do Kleinian Variation</source>
        <translation type="unfinished">Variante Pseu&amp;do Kleinian</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2836"/>
        <source>Pseudo Kleinian Parameters</source>
        <translation type="unfinished">Parametri Pseudo Kleinian</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2849"/>
        <source>Size R2:</source>
        <translation type="unfinished">Dimensioni R2:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="2940"/>
        <source>Csize:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3073"/>
        <source>DEadd (tweak):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3111"/>
        <source>DE factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>z.z  subtr. scale:</source>
        <translation type="obsolete">z.z  subtr. scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3267"/>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3987"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3463"/>
        <source>Men&amp;ger Sponge</source>
        <translation type="unfinished">Spugna di Men&amp;ger</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3647"/>
        <source>Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3877"/>
        <source>Menger Scale Factor:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="3974"/>
        <source>Analytic Logrithmic DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_pseudo_kleinian3.ui" line="4201"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>quaternion</name>
    <message>
        <location filename="../qt_data/fractal_quaternion.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion.ui" line="42"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>quaternion3D</name>
    <message>
        <location filename="../qt_data/fractal_quaternion3D.ui" line="100"/>
        <source>DE Vector Influence:</source>
        <translation type="unfinished">DE Influenza Vettore:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion3D.ui" line="107"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion3D.ui" line="114"/>
        <source>In&amp;ternal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion3D.ui" line="415"/>
        <source>Quaternion Scales</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion3D.ui" line="652"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>quaternion_4D</name>
    <message>
        <location filename="../qt_data/fractal_quaternion_4D.ui" line="275"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion_4D.ui" line="311"/>
        <source>Quaternion Scales</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion_4D.ui" line="543"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion_4D.ui" line="585"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion_4D.ui" line="592"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quaternion_4D.ui" line="950"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>quick_dudley_mod</name>
    <message>
        <location filename="../qt_data/fractal_quick_dudley_mod.ui" line="104"/>
        <source>Multiplier</source>
        <translation type="unfinished">Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quick_dudley_mod.ui" line="225"/>
        <source>Sq-Multiplier</source>
        <translation type="unfinished">Sq-Moltiplicatore</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quick_dudley_mod.ui" line="412"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>quickdudley</name>
    <message>
        <location filename="../qt_data/fractal_quickdudley.ui" line="35"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_quickdudley.ui" line="42"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>riemann_bulb_msltoe_mod2</name>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="35"/>
        <source>Use bailout between 1.0 and 2.4</source>
        <translation type="unfinished">Usare bailout tra 1.0 e 2.4</translation>
    </message>
    <message>
        <source>Sine Function  Scale</source>
        <translation type="obsolete">Scala Funzione Seno</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="139"/>
        <source>Sine Function Scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="146"/>
        <source>Periodic Cubic Tiling</source>
        <translation type="unfinished">Copertura pattern periodico cubico</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="286"/>
        <source>Sine function</source>
        <translation type="unfinished">Funzione Seno</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="321"/>
        <source>Radius^2 &lt; Minimum Radius^2 Parameters</source>
        <translation type="unfinished">Parametri Raggio^2 &lt; Raggio minimo^2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="335"/>
        <source>Scale (not smooth inside)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="405"/>
        <source>Second Offset</source>
        <translation type="unfinished">Secondo Offset</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="527"/>
        <source>Second Scale</source>
        <translation type="unfinished">Seconda Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="611"/>
        <source>Fudge Factor</source>
        <translation type="unfinished">Fattore Fudge</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="646"/>
        <source>First Offset</source>
        <translation type="unfinished">Primo Offset</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="675"/>
        <source>First Scale</source>
        <translation type="unfinished">Prima Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="682"/>
        <source>Radius^2 &gt; Minimum Radius^2 Parameters</source>
        <translation type="unfinished">Parametri Raggio^2 &lt; Raggio minimo^2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="721"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="803"/>
        <source>Smooth Inside</source>
        <translation type="unfinished">modalità non rigida interna</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_bulb_msltoe_mod2.ui" line="812"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>riemann_sphere_msltoe</name>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoe.ui" line="41"/>
        <source>Riemann Sphere Msltoe</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoe.ui" line="48"/>
        <source>In&amp;ternal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoe.ui" line="241"/>
        <source>Scale and Offsets:</source>
        <translation type="unfinished">Scala e Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoe.ui" line="253"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoe.ui" line="353"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoe.ui" line="497"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>riemann_sphere_msltoeV1</name>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoeV1.ui" line="41"/>
        <source>Riemann Sphere Msltoe Variation1</source>
        <translation type="unfinished">Sfera di Riemann Msltoe  Variante1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoeV1.ui" line="109"/>
        <source>Constant Multiplier</source>
        <translation type="unfinished">Moltiplicatore Costanti</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoeV1.ui" line="183"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoeV1.ui" line="288"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoeV1.ui" line="505"/>
        <source>In&amp;ternal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_riemann_sphere_msltoeV1.ui" line="692"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>sierpinski_3D</name>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="93"/>
        <source>Sierpinski scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="109"/>
        <source>normal</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="116"/>
        <source>reverse</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="226"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="233"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="240"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="395"/>
        <source>Rotat&amp;ion 3D</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="442"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="449"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="665"/>
        <source>DE tweak temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_3D.ui" line="727"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>sierpinski_4D</name>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="93"/>
        <source>Sierpinski scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="141"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="148"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="155"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="421"/>
        <source>Rotat&amp;ion 6 planes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="463"/>
        <source>Stop Rotation at iteration:</source>
        <translation type="unfinished">ferma rotazione a iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="470"/>
        <source>Start Rotation at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="830"/>
        <source>DE tweak temp</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_sierpinski_4D.ui" line="892"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>smooth_mandelbox</name>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="35"/>
        <source>Main parameters</source>
        <translation type="unfinished">Parametri principali</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="61"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="68"/>
        <source>Spherical folding
fixed radius:</source>
        <translation type="unfinished">Raggio fisso
Folding sferico: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="186"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="193"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="312"/>
        <source>Spherical folding
min radius:</source>
        <translation type="unfinished">Raggio minimo 
Folding sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="348"/>
        <source>Sharpness:</source>
        <translation type="unfinished">Dettaglio angoli:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="410"/>
        <source>&amp;Main rotation</source>
        <translation type="unfinished">Rotazione &amp;principale</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="597"/>
        <source>Coloring parameters</source>
        <translation type="unfinished">Parametri colorazione</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="623"/>
        <source>Min radius component:</source>
        <translation type="unfinished">Componente raggio Min:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="630"/>
        <source>Y plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="737"/>
        <source>X plane component:</source>
        <translation type="unfinished">Componente piano Y:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="744"/>
        <source>Z plane component:</source>
        <translation type="unfinished">componente piano Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="823"/>
        <source>Absolute value of z:</source>
        <translation type="unfinished">Valore assoluto di z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="858"/>
        <source>Fixed radius component:</source>
        <translation type="unfinished">Componente raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_smooth_mandelbox.ui" line="970"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>transf_add_Cpixel</name>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel.ui" line="38"/>
        <source>addCpixel ConstantMultiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel.ui" line="230"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel.ui" line="239"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>transf_add_Cpixel_CxCy_axis_swap</name>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_CxCy_axis_swap.ui" line="54"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_CxCy_axis_swap.ui" line="57"/>
        <source>addCpixel c.x c.y axis swap ConstantMultiplier</source>
        <translation type="unfinished">addCpixel c.x c.y Scambio Assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_CxCy_axis_swap.ui" line="64"/>
        <source>c.x c.y swap enabled</source>
        <translation type="unfinished">Scambio c.x c.y abilitato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_CxCy_axis_swap.ui" line="181"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_CxCy_axis_swap.ui" line="244"/>
        <source>alternate c.x c.y swap</source>
        <translation type="unfinished">alterna scambio c.x c.y</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_CxCy_axis_swap.ui" line="253"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_add_Cpixel_axis_swap</name>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_axis_swap.ui" line="206"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_axis_swap.ui" line="257"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_axis_swap.ui" line="271"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_axis_swap.ui" line="280"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_add_Cpixel_symmetrical</name>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_symmetrical.ui" line="47"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_symmetrical.ui" line="250"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_add_Cpixel_varyV1</name>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_varyV1.ui" line="375"/>
        <source>addCpixel multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="obsolete">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_varyV1.ui" line="430"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_varyV1.ui" line="437"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_varyV1.ui" line="446"/>
        <source>Variable</source>
        <translation type="unfinished">Variabile</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_Cpixel_varyV1.ui" line="462"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_add_exp2_Z</name>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="47"/>
        <source>negative fabs(z.z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="82"/>
        <source>scale z.x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="89"/>
        <source>DE Tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="96"/>
        <source>negative fabs(z.y)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="103"/>
        <source>enable exp2(z.x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="142"/>
        <source>enable exp2(z.y)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="234"/>
        <source>enable exp2(z.z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="241"/>
        <source>scale z.y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="248"/>
        <source>scale z.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="403"/>
        <source>fabs(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="325"/>
        <source>negative fabs(z.x)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="380"/>
        <source>Version with analytic DE calculation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="389"/>
        <source>scale z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="396"/>
        <source>negative fabs(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="438"/>
        <source>enable exp2(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_add_exp2_Z.ui" line="472"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>transf_addition_constant</name>
    <message>
        <source>Addition Constant 1  - Julia Constant</source>
        <translation type="obsolete">Aggiunta Costante 1 - Costante di Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="41"/>
        <source>Addition Constant 1 - Julia Constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="97"/>
        <source>Julia constant</source>
        <translation type="unfinished">Costante di Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="113"/>
        <source>Get Point Coodinates with mouse pointer, then use right mouse click copy /paste vector to enter Julia constant.</source>
        <translation type="unfinished">Cattura Coodinate tramite puntatore del mouse, poi usa la funzione copia/incolla del tasto destro del mouse per incollare i valori relativi alla costante di Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="129"/>
        <source>Addition Constant 2 - Offset</source>
        <translation type="unfinished">Aggiunta Costante 2 - Costante di Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="191"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="321"/>
        <source>Offset. Use in normal mode or use to explore in Julia mode</source>
        <translation type="unfinished">Offset Da usare in modo normale o per esplorare modalità Julia</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant.ui" line="328"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_addition_constant_4D</name>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_4D.ui" line="132"/>
        <source>Addition Constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_4D.ui" line="284"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_addition_constant_varyV1</name>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_varyV1.ui" line="375"/>
        <source>Addition Constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="obsolete">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_varyV1.ui" line="430"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_varyV1.ui" line="437"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_varyV1.ui" line="446"/>
        <source>Variable</source>
        <translation type="unfinished">Variabile</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_addition_constant_varyV1.ui" line="462"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_benesi_T1</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1.ui" line="389"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1.ui" line="396"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1.ui" line="405"/>
        <source>Mag Transform &amp;T1 Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1.ui" line="599"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_benesi_T1Mod</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1Mod.ui" line="119"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1Mod.ui" line="290"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1Mod.ui" line="440"/>
        <source>Mag Transform &amp;T1Mod Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T1Mod.ui" line="627"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_benesi_T2</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T2.ui" line="153"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T2.ui" line="160"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T2.ui" line="412"/>
        <source>Mag Transform &amp;T2 Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T2.ui" line="599"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_benesi_T3</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T3.ui" line="154"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T3.ui" line="346"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T3.ui" line="412"/>
        <source>Mag Transform &amp;T3 Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T3.ui" line="599"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_benesi_T4</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T4.ui" line="169"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T4.ui" line="198"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T4.ui" line="412"/>
        <source>Mag Transform &amp;T4 Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T4.ui" line="599"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_benesi_T5b</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T5b.ui" line="89"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T5b.ui" line="508"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T5b.ui" line="608"/>
        <source>Power A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T5b.ui" line="622"/>
        <source>Power B</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T5b.ui" line="669"/>
        <source>Mag Transform &amp;T5b Internal Rotation;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_T5b.ui" line="856"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_benesi_cube_sphere</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_cube_sphere.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Cube to Sphere transform&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;Description:Warpsacubetoasphere;transformmadebyM.Benesi,optimizedbyLuca&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;http://www.fractalforums.com/mathematics/circle2square/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Trasformazione Cubo in Sfera&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;Descrizione: Cambia la forma di un oggetto cubico  in quella di un oggetto sferico, formula di M.Benesi, ottimizzata da Dark Beam&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;http://www.fractalforums.com/mathematics/circle2square/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_cube_sphere.ui" line="38"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_cube_sphere.ui" line="45"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>transf_benesi_mag_backward</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_mag_backward.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_mag_backward.ui" line="38"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_mag_backward.ui" line="45"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_benesi_mag_forward</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_mag_forward.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#000000;&quot;&gt;//http://www.fractalforums.com/new-theories-and-research/do-m3d-formula-have-to-be-distance-estimation-formulas/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_mag_forward.ui" line="38"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_mag_forward.ui" line="45"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_benesi_sphere_cube</name>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_sphere_cube.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#008000;&quot;&gt;Sphere&lt;/span&gt;&lt;span style=&quot; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; color:#008000;&quot;&gt;to&lt;/span&gt;&lt;span style=&quot; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; color:#008000;&quot;&gt;Cube&lt;/span&gt;&lt;span style=&quot; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; color:#008000;&quot;&gt;transform&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;Warps&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;a&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;sphere&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;to&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;a&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;cube;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;transform&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;made&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;by&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;M.Benesi,&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;optimized&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;by Luca.&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;Scavenged&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;and&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;edited&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;from&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;code&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;optimized&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;by&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#c0c0c0;&quot;/&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;Luca.&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New,courier&apos;; color:#008000;&quot;&gt;http://www.fractalforums.com/mathematics/circle2square/&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_sphere_cube.ui" line="38"/>
        <source>No parameters</source>
        <translation type="unfinished">Non ci sono parametri</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_benesi_sphere_cube.ui" line="45"/>
        <source>Infor&amp;mation</source>
        <translation type="unfinished">Infor&amp;mazione</translation>
    </message>
</context>
<context>
    <name>transf_box_fold</name>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold.ui" line="40"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold.ui" line="69"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold.ui" line="206"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold.ui" line="224"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_box_fold_4D</name>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_4D.ui" line="40"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_4D.ui" line="47"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_4D.ui" line="163"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_box_fold_XYZ</name>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_XYZ.ui" line="296"/>
        <source>Folding Limit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_XYZ.ui" line="303"/>
        <source>Folding Value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_XYZ.ui" line="405"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_box_fold_varyV1</name>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="40"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="121"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="158"/>
        <source>Value/Limit Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="257"/>
        <source>DE tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="264"/>
        <source>Vary Limit:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="340"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="347"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_fold_varyV1.ui" line="394"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_box_offset</name>
    <message>
        <location filename="../qt_data/fractal_transf_box_offset.ui" line="104"/>
        <source>Box Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_offset.ui" line="227"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Can sometimes improve render quality when using analytic DE&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A volte può migliorare la qualità di renering usando la DE analitica&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_offset.ui" line="230"/>
        <source>Analytic DE calculation</source>
        <translation type="unfinished">Calcolo DE analitica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_box_offset.ui" line="239"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_fabs_add_conditional</name>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional.ui" line="45"/>
        <source>z = SIGN(z) (offset - fabs(z) + fabs(z) * Slope)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional.ui" line="64"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional.ui" line="219"/>
        <source>Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional.ui" line="491"/>
        <source>DE Tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_conditional.ui" line="507"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>transf_fabs_add_constant</name>
    <message>
        <source>fABS Add Constant  z = fabs( z + constant)</source>
        <translation type="obsolete">fABS Aggiunge Costante  z = fabs( z + constant)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant.ui" line="35"/>
        <source>fABS Add Constant z = fabs( z + constant)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant.ui" line="47"/>
        <source>fABS addition constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant.ui" line="402"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant.ui" line="471"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_fabs_add_constantV2</name>
    <message>
        <source>fABS Add Constant V2  z = fabs( z + constant) - fabs(  z - constant) - z;</source>
        <translation type="obsolete">fABS Aggiunge Costante V2  z = fabs(z + constant) - fabs(z - constant) - z;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="35"/>
        <source>fABS Add Constant V2 z = fabs( z + constant) - fabs( z - constant) - z;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="193"/>
        <source>fABS addition constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="239"/>
        <source>enable Tglad Tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="246"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="253"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="327"/>
        <source>Tweak Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2.ui" line="477"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_fabs_add_constantV2_4D</name>
    <message>
        <source>fABS Add Constant V2  4D     z = fabs( z + constant) - fabs(  z - constant) - z;</source>
        <translation type="obsolete">fABS Aggiunge Costante V2  4D     z = fabs(z + constant) - fabs(z - constant) - z;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2_4D.ui" line="35"/>
        <source>fABS Add Constant V2 4D z = fabs( z + constant) - fabs( z - constant) - z;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2_4D.ui" line="121"/>
        <source>fABS addition constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constantV2_4D.ui" line="305"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_fabs_add_constant_4D</name>
    <message>
        <source>fABS Add Constant  4D        z = fabs( z + constant)</source>
        <translation type="obsolete">fABS Aggiunge Costante  4D        z = fabs(z + constant)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant_4D.ui" line="35"/>
        <source>fABS Add Constant 4D z = fabs( z + constant)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant_4D.ui" line="54"/>
        <source>fABS addition constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_constant_4D.ui" line="313"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_fabs_add_multi</name>
    <message>
        <source>fABS Add Constant   Multi   z = fabs(  z + const.A ) - fabs(  z - const.B )  - (  z * const.C  )</source>
        <translation type="obsolete">fABS Aggiunge Multi Costante   z = fabs(z + const.A) - fabs(z - const.B)  - (z * const.C)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_multi.ui" line="195"/>
        <source>Enable fABS(z - B)</source>
        <translation type="unfinished">Abilita fABS(z - B)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_multi.ui" line="41"/>
        <source>fABS Add Constant Multi z = fabs( z + const.A ) - fabs( z - const.B ) - ( z * const.C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_multi.ui" line="369"/>
        <source>- ( z * C )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_multi.ui" line="511"/>
        <source>Enable fABS(z + A)</source>
        <translation type="unfinished">Abilita fABS(z + A)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_multi.ui" line="767"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_fabs_add_multi.ui" line="836"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_folding_tetra3D</name>
    <message>
        <location filename="../qt_data/fractal_transf_folding_tetra3D.ui" line="35"/>
        <source>1st half tetrahedral symmetry planes:</source>
        <translation type="unfinished">Piani simmetria tetraedro prima metà:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_folding_tetra3D.ui" line="42"/>
        <source>2nd half tetrahedral symmetry planes:</source>
        <translation type="unfinished">Piani simmetria tetraedro seconda metà:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_folding_tetra3D.ui" line="49"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_inv_cylindrical</name>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="47"/>
        <source>z scale:</source>
        <translation type="unfinished">scala z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="54"/>
        <source>Enable newZy = z.x * sin(z.y)</source>
        <translation type="unfinished">Abilita newZy = z.x * sin(z.y)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="61"/>
        <source>Enable newZx = z.x * cos(z.y)</source>
        <translation type="unfinished">Abilita newZx = z.x * cos(z.y)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="93"/>
        <source>z.y Multiplier::</source>
        <translation type="unfinished">Moltiplicatore z.y  </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="188"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="258"/>
        <source>Analytic DE Muliplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="389"/>
        <source>Analytic DE Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_inv_cylindrical.ui" line="405"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_iteration_weight</name>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="35"/>
        <source>Iteration Weight.; z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="166"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="173"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="202"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="209"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="216"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="264"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight.ui" line="280"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_iteration_weight_4D</name>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="35"/>
        <source>Iteration Weight 4D z = ( z * const.Z) + ( zA * const.A) + ( zB * const.B)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="166"/>
        <source>Constant B:</source>
        <translation type="unfinished">Costante B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="173"/>
        <source>zB after itr.:</source>
        <translation type="unfinished">zB dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="202"/>
        <source>Constant A:</source>
        <translation type="unfinished">Constante A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="209"/>
        <source>Constant Z:</source>
        <translation type="unfinished">Constante Z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="216"/>
        <source>zA after itr.:</source>
        <translation type="unfinished">zA dopo itr.:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="264"/>
        <source>Normal use : Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Normal use :  Constants A &amp; B are negative numbers, Constant Z remains at 1.0</source>
        <translation type="obsolete">Uso normale :  Le costanti A e B sono numeri negativi, la costante Z rimane 1.0</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_iteration_weight_4D.ui" line="280"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
<context>
    <name>transf_lin_combine_Cxyz</name>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="341"/>
        <source>CX Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="527"/>
        <source>CY Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="534"/>
        <source>CZ Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="606"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="609"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="845"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="855"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="869"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="881"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="884"/>
        <source>Suggestion:
=You can use it for example as a pre-transform for bulbs or boxes.</source>
        <translation type="unfinished">Suggerimento:
Puoi usarlo ad esempio come pre-transformazione per mandelbulb o mandelbox.</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_lin_combine_Cxyz.ui" line="892"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_menger_fold</name>
    <message>
        <source>&amp;Menger Sponge Start Function     Fabs function z = fabs( z + const.A);</source>
        <translation type="obsolete">Inizia Funzione Spugna di &amp;Menger     Fabs funzione z = fabs(z + const.A) </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="35"/>
        <source>&amp;Menger Sponge Start Function Fabs function z = fabs( z + const.A);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="114"/>
        <source>Const.A y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="181"/>
        <source>Const.A x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="188"/>
        <source>Const.A z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="251"/>
        <source>&amp;Menger Sponge End Function Apply Constants;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="515"/>
        <source>Conditional mode: if ( z.z &lt; limit ) z.z = z.z;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="380"/>
        <source>Menger Sponge scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="416"/>
        <source>Offset Multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_menger_fold.ui" line="534"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_multiple_angle</name>
    <message>
        <location filename="../qt_data/fractal_transf_multiple_angle.ui" line="93"/>
        <source>Angle multiplication:</source>
        <translation type="unfinished">Moltiplicazione angolo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_multiple_angle.ui" line="102"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_neg_fabs_add_constant</name>
    <message>
        <source>Neg fABS Add Constant  z =  - fabs( z + constant)</source>
        <translation type="obsolete">Neg fABS Aggiunfi Costante  z =  - fabs(z + constant)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_neg_fabs_add_constant.ui" line="35"/>
        <source>Neg fABS Add Constant z = - fabs( z + constant)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_neg_fabs_add_constant.ui" line="47"/>
        <source>Addition constant</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_neg_fabs_add_constant.ui" line="252"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_octo_fold</name>
    <message>
        <location filename="../qt_data/fractal_transf_octo_fold.ui" line="93"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_octo_fold.ui" line="157"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_octo_fold.ui" line="287"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_pwr2_polynomial</name>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="97"/>
        <source>fabs(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>fn(z)  = Pi Rotation about Vector A</source>
        <translation type="obsolete">fn(z)  = Rotazione Pi attorno al vettore A</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="136"/>
        <source>Pi Rotation:</source>
        <translation type="unfinished">Rotazione Pi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="264"/>
        <source>Analytic Linear DE offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PART A   =  - (ScaleA * z + VectA)  *  fn(z)</source>
        <translation type="obsolete">PARTE A   =  - (ScaleA * z + VectA)  *  fn(z)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="389"/>
        <source>Final Scale:</source>
        <translation type="unfinished">Scala Finale:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="396"/>
        <source>fabs(z):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Power2 Polynomial   Az^2 + Bz + C</source>
        <translation type="obsolete">Power2 Polinomiale Az^2 + Bz + C</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="107"/>
        <source>fn(z) = Pi Rotation about Vector A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="382"/>
        <source>PART A = - (ScaleA * z + VectA) * fn(z)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="438"/>
        <source>Power2 Polynomial Az^2 + Bz + C</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="471"/>
        <source>Scale B:</source>
        <translation type="unfinished">Scala B:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="504"/>
        <source>Vector C:</source>
        <translation type="unfinished">Vettore C:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="511"/>
        <source>Analytic Linear DE multiplier:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="532"/>
        <source>PART B = ScaleB * z + VectorC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="718"/>
        <source>fn(z) = Box Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="1039"/>
        <source>fn( z) options, (default = z;)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="1049"/>
        <source>fn( z) = cos ( z * pi )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>PART B =  ScaleB *  z  + VectorC</source>
        <translation type="obsolete">PARTE B =  ScaleB *  z  + VectorC</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="621"/>
        <source>Scale A:</source>
        <translation type="unfinished">Scala A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="635"/>
        <source>Power3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="642"/>
        <source>fabs(fn(z)):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="828"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="967"/>
        <source>Vector A:</source>
        <translation type="unfinished">Vettore A:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="1078"/>
        <source>Pi Scale:</source>
        <translation type="unfinished">Scala Pi:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_pwr2_polynomial.ui" line="1145"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_quaternion_fold</name>
    <message>
        <source>Enable Sca&amp;le and Offset controls; (  iterates only the quaternion fold when disabled.)</source>
        <translation type="obsolete">Abilita controlli Sca&amp;la e Offset  (itera soltanto il Folding del quaternione quando disabilitato)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="35"/>
        <source>Enable Sca&amp;le and Offset controls; ( iterates only the quaternion fold when disabled.)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="227"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="428"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="487"/>
        <source>DE Vector Influence:</source>
        <translation type="unfinished">DE Influenza Vettore:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="499"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel is CVector3 c = ( c.x, c.y, c.z). It is each original point (pixel) in 3D space where the iteration starts from, ( c = original z for each point .) It is commonly found in formula structures z = f(z) + c.&lt;/p&gt;&lt;p&gt;The Cpixel multiplier effectively scales this point &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;This transform allows the swapping of the order of the &amp;quot;CVector3 c&amp;quot; components before being multiplied by &amp;quot;CVector3 z&amp;quot; components.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Cpixel è formato da CVector3 c = ( c.x, c.y, c.z). Questo è formato da ciascun punto originale (pixel) nello spazio 3D, da cui ha inizio l&apos;iterazione ( c = origine z per ogni punto) normalmente si trova nella struttura delle formule z = f(z) + c.&lt;/p&gt;&lt;p&gt;Il moltiplicatore Cpixel scala effettivamente questo punto &amp;quot;c&amp;quot;.&lt;/p&gt;&lt;p&gt;Questa trasformazione permette lo scambio di ordine dei componenti di &amp;quot;CVector3 c&amp;quot; prima di venir moltiplicati per i componenti di &amp;quot;CVector3 z&amp;quot;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="502"/>
        <source>addCpi&amp;xel Axis Swap ConstantMultiplier</source>
        <translation type="unfinished">addCpi&amp;xel scambio assi ConstantMultiplier</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="738"/>
        <source>function order of c.x, c.y and c.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="748"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="762"/>
        <source>alternate swap</source>
        <translation type="unfinished">scambio alternato</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_quaternion_fold.ui" line="774"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_rPow3</name>
    <message>
        <location filename="../qt_data/fractal_transf_rPow3.ui" line="68"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rPow3.ui" line="100"/>
        <source>Analytical Linear DE adjustment offset</source>
        <translation type="unfinished">Ritocco offset DE Lineare Analitica</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rPow3.ui" line="137"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_reciprocal3</name>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="106"/>
        <source>2 limit &amp; 2 mult. mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="190"/>
        <source>LimitB:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="197"/>
        <source>DE Tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="274"/>
        <source>Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="480"/>
        <source>3 limit &amp; 2 mult. mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="639"/>
        <source>LimitA:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="646"/>
        <source>MultiplierA:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="113"/>
        <source>z = SIGN(z) ((1.0/LimitA ) - (1.0/(fabs(z) +LimitA)) + (fabs(z) * Slope))</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="127"/>
        <source>z = SIGN(z) (LimitC - 1/(fabs(z*MultA)+LimA) -1/(z*z*MultB+ LimB) +(fabs(z) * Slope))</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="607"/>
        <source>z = SIGN(z) ((1/LimA )+(1/LimB) - 1/(fabs(z*MultA)+LimA) -1/(z*z*MultB+ LimB) etc)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="660"/>
        <source>z = SIGN(z) (LimitB - (1.0/(fabs(z) +LimitA)) + (fabs(z) * Slope))</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="741"/>
        <source>1 limit mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="748"/>
        <source>2 limit mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="938"/>
        <source>LimitC:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="970"/>
        <source>MultiplierB:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_reciprocal3.ui" line="1078"/>
        <source>I&amp;nformation</source>
        <translation type="unfinished">I&amp;nformazione</translation>
    </message>
</context>
<context>
    <name>transf_rotated_folding</name>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="35"/>
        <source>Pre-Rotate , choose a fold, post-rotate back</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="49"/>
        <source>Rotation Forward</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="243"/>
        <source>fabs fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="250"/>
        <source>z = fabs(z + offset) - offset;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="315"/>
        <source>Offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="451"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="522"/>
        <source>Tglad Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="542"/>
        <source>z = fabs(z + offset) - fabs( z - offset ) - z;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="784"/>
        <source>z.z scale:</source>
        <translation type="unfinished">scala z.z:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="856"/>
        <source>if ( z &gt;Limit) z = Value -z, else if( z &lt;Limit) z = - Value - z,</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="573"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="703"/>
        <source>MBox Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="793"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="879"/>
        <source>Enable Axis</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="917"/>
        <source>Rotation Back</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotated_folding.ui" line="1084"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_rotation</name>
    <message>
        <location filename="../qt_data/fractal_transf_rotation.ui" line="195"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_rotation_4D</name>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_4D.ui" line="369"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_rotation_folding_plane</name>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="41"/>
        <source>Rotation of separate folding planes</source>
        <translation type="unfinished">Rotazione Piani di Folding separati</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="107"/>
        <source>Box folding value:</source>
        <translation type="unfinished">Valore di Box folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="142"/>
        <source>Box folding limit:</source>
        <translation type="unfinished">Limite box Folding:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="185"/>
        <source>X Axis</source>
        <translation type="unfinished">Asse X</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="251"/>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="615"/>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="1073"/>
        <source>Positive plane</source>
        <translation type="unfinished">Piano positivo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="338"/>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="625"/>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="1083"/>
        <source>Negative plane</source>
        <translation type="unfinished">Piano negativo</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="554"/>
        <source>Y Axis</source>
        <translation type="unfinished">Asse Y</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="923"/>
        <source>Z Axis</source>
        <translation type="unfinished">Asse Z</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_folding_plane.ui" line="1279"/>
        <source>Informa&amp;tion</source>
        <translation type="unfinished">Informa&amp;zione</translation>
    </message>
</context>
<context>
    <name>transf_rotation_varyV1</name>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="obsolete">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_varyV1.ui" line="248"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_varyV1.ui" line="255"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_varyV1.ui" line="334"/>
        <source>Variable</source>
        <translation type="unfinished">Variabile</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rotation_varyV1.ui" line="444"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_rpower</name>
    <message>
        <location filename="../qt_data/fractal_transf_rpower.ui" line="93"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_rpower.ui" line="102"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_scale</name>
    <message>
        <location filename="../qt_data/fractal_transf_scale.ui" line="93"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale.ui" line="102"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_scale_3D</name>
    <message>
        <location filename="../qt_data/fractal_transf_scale_3D.ui" line="220"/>
        <source>Scale</source>
        <translation type="unfinished">Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_3D.ui" line="229"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_scale_4D</name>
    <message>
        <location filename="../qt_data/fractal_transf_scale_4D.ui" line="93"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_4D.ui" line="102"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_scale_varyV1</name>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyV1.ui" line="35"/>
        <source>Scale_Vary_V1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="obsolete">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyV1.ui" line="47"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyV1.ui" line="170"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyV1.ui" line="190"/>
        <source>Scale Variable:</source>
        <translation type="unfinished">Variabile Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyV1.ui" line="197"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyV1.ui" line="219"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_scale_varyVCL</name>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="47"/>
        <source>0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="35"/>
        <source>Scale_Vary_VCL Vary Curvilinear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="76"/>
        <source>Curvi funtion ( disable to use linear slopes only)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="149"/>
        <source>B Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="169"/>
        <source>C Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="229"/>
        <source>Iteration A:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="236"/>
        <source>Iteration C:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="243"/>
        <source>Iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="250"/>
        <source>Intitial Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="257"/>
        <source>Iteration B:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="292"/>
        <source>Curvilinear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="327"/>
        <source>A Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="375"/>
        <source>Note. Curvi Function requires the condition that B &gt; 2 * A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="549"/>
        <source>Parabolic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="556"/>
        <source>Initial Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="563"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="570"/>
        <source>The Curvilinear and Parabolic functions can be applied together</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="577"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_scale_varyVCL.ui" line="593"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherFold_varyVCL</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="35"/>
        <source>Spherical Fold - Iteration based varying minR2 parameter</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="72"/>
        <source>Initial minR2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="107"/>
        <source>Fixed maxR2:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="215"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="321"/>
        <source>Post_scale</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="520"/>
        <source>Iteration B:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="555"/>
        <source>Curvilinear</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="616"/>
        <source>A Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="455"/>
        <source>Parameters for varying minR2 in respect to iteration count</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="568"/>
        <source>Note. Curvi Function requires the condition that B &gt; 2 * A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="667"/>
        <source>Curvi funtion ( disable to use linear slopes only)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="740"/>
        <source>B Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="760"/>
        <source>0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="767"/>
        <source>C Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="774"/>
        <source>Iteration A:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="781"/>
        <source>Iteration C:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="788"/>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="976"/>
        <source>Initial add:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="795"/>
        <source>Iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="969"/>
        <source>Parabolic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="983"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="990"/>
        <source>The Curvilinear and Parabolic functions can be applied together</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="997"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherFold_varyVCL.ui" line="1006"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_sphere_inv</name>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="35"/>
        <source>SphereInv from M3D</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="103"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="231"/>
        <source>Pre_scale</source>
        <translation type="unfinished">Pre_Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="313"/>
        <source>Pre_dot(z,z)</source>
        <translation type="unfinished">Pre_Prodotto(z,z)</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="322"/>
        <source>No mode = unconditional spherical inversion z = z / r2;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="472"/>
        <source>let r2 = Radius2 (conformal - undo the inversion inside a sphere)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>No mode =  unconditional spherical inversion  z = z / r2;</source>
        <translation type="obsolete">no modale = inversione sferica senza condizione z = z / r2;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="338"/>
        <source>let r2 = 2 * Radius2 - r2 (non conformal &quot;reversal&quot; inversion)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="373"/>
        <source>Mode Radius2:</source>
        <translation type="unfinished">Modo Radius2:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="458"/>
        <source>Mode Radius1:</source>
        <translation type="unfinished">Modo Radius1:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="465"/>
        <source>Mode 1</source>
        <translation type="unfinished">Modo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="479"/>
        <source>Mode 2</source>
        <translation type="unfinished">Modo 2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="500"/>
        <source>Adjust Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_inv.ui" line="687"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_sphere_invC</name>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_invC.ui" line="213"/>
        <source>Cpixel Multiplier</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_invC.ui" line="232"/>
        <source>Suggestion:
Use as pre-transform for one iteration, 
will invert the complete coord system.
Cut off at Z=0 to see the inside.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_sphere_invC.ui" line="242"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_fold</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold.ui" line="35"/>
        <source>Spherical Fold - MBox</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold.ui" line="44"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold.ui" line="51"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold.ui" line="193"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold.ui" line="377"/>
        <source>Pre_scale</source>
        <translation type="unfinished">Pre_Scala</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold.ui" line="439"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_fold_4D</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_4D.ui" line="35"/>
        <source>Spherical Fold_4D</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_4D.ui" line="64"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_4D.ui" line="71"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_4D.ui" line="196"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_fold_abox</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_abox.ui" line="35"/>
        <source>Spherical Fold - ABox/ASurf</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_abox.ui" line="72"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_abox.ui" line="108"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_abox.ui" line="316"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_fold_varyV1</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="51"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="118"/>
        <source>Fixed radius variable:</source>
        <translation type="unfinished">Variabile raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="153"/>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="389"/>
        <source>Start varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Finish varying  at iteration:</source>
        <translation type="obsolete">Ferma variazione all&apos;iterazione: </translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="35"/>
        <source>Spherical Fold, Variable Radius Controls - MBox type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="186"/>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="350"/>
        <source>Finish varying at iteration:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="242"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="382"/>
        <source>Minimum radius variable:</source>
        <translation type="unfinished">Variabile raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="436"/>
        <source>Spherical folding
offset:</source>
        <translation type="unfinished">Offset Folding
Sferico:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_fold_varyV1.ui" line="644"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_offset</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offset.ui" line="68"/>
        <source>Scale:</source>
        <translation type="unfinished">Scala:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offset.ui" line="103"/>
        <source>Offset Radius:</source>
        <translation type="unfinished">Raggio Offset:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offset.ui" line="169"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_offsetVCL</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="44"/>
        <source>Curvilinear Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="58"/>
        <source>Curvilinear slopes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="35"/>
        <source>The Curvilinear, Parabolic &amp; Dot modes can be applied consecutively</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="51"/>
        <source>Curvi function</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="84"/>
        <source>Note. Curvi Function requires the condition that B &gt; 2 *A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="106"/>
        <source>Parameters for tweaking value of radial offset in respect to iteration count</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="118"/>
        <source>C Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="200"/>
        <source>A Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="207"/>
        <source>Iteration A:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="214"/>
        <source>Iteration B:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="234"/>
        <source>Iteration C:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="248"/>
        <source>0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="283"/>
        <source>Iteration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="350"/>
        <source>B Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="426"/>
        <source>Init. Value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="475"/>
        <source>Axis Slope:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="510"/>
        <source>Parabolic scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="593"/>
        <source>Parabolic Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="628"/>
        <source>Initial offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="646"/>
        <source>z *= 1.0 + offset/ -z.Dot(z);</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="653"/>
        <source>Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="660"/>
        <source>Dot Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="791"/>
        <source>Post_scale vec(z):</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="844"/>
        <source>Lin. DE Offset:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="851"/>
        <source>Analytical DE tweaks</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="939"/>
        <source>Lin. DE Scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_offsetVCL.ui" line="948"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_spherical_pwr_fold</name>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_pwr_fold.ui" line="35"/>
        <source>Spherical Power Fold</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_pwr_fold.ui" line="120"/>
        <source>Fixed Radius:</source>
        <translation type="unfinished">Raggio fisso:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_pwr_fold.ui" line="152"/>
        <source>Minimum Radius:</source>
        <translation type="unfinished">Raggio minimo:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_pwr_fold.ui" line="284"/>
        <source>Power A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_pwr_fold.ui" line="291"/>
        <source>Power B</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_spherical_pwr_fold.ui" line="310"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_surf_box_fold</name>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="48"/>
        <source>Fold z.x</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Type 4 Fold Value</source>
        <translation type="obsolete">Tipo 4 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="325"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="452"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1062"/>
        <source>Type 4 Fold value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="332"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="545"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1076"/>
        <source>Type 3</source>
        <translation type="unfinished">Tipo 3</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="339"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="516"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="812"/>
        <source>Type 1</source>
        <translation type="unfinished">Tipo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="346"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="678"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1069"/>
        <source>Type 4</source>
        <translation type="unfinished">Tipo 4</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="353"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="752"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1131"/>
        <source>Type 2</source>
        <translation type="unfinished">Tipo 2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="360"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="445"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1124"/>
        <source>Type 5 Fold2 value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="786"/>
        <source>Type 4 z.z scale:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Type 5 Fold2 Value</source>
        <translation type="obsolete">Tipo 5 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="367"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="726"/>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="983"/>
        <source>Type 5</source>
        <translation type="unfinished">Tipo 5</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="411"/>
        <source>Fold z.y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="774"/>
        <source>Fold z.z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1254"/>
        <source>DE Tweak</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_box_fold.ui" line="1270"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_surf_fold_multi</name>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Based on amazing_surf_mod1  from Mandelbulb3D&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="obsolete">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Baseata sulla formula amazing_surf_mod1 da Mandelbulb3D&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="38"/>
        <source>FoldBox</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="67"/>
        <source>Fold z.y:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="96"/>
        <source>Fold z.x:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="110"/>
        <source>z = fold - fabs( fabs(z) - fold)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Type 4 Fold Value</source>
        <translation type="obsolete">Tipo 4 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="35"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; color:#000000;&quot;&gt;Based on amazing_surf_mod1 from Mandelbulb3D&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="103"/>
        <source>if ( z &gt; Fold) z = Value -z, else if( z &lt;Fold) z = - Value - z,</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="117"/>
        <source>Type 4 Fold value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="143"/>
        <source>z = fabs(z + fold) - fabs(z - fold) -z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="150"/>
        <source>Type 3</source>
        <translation type="unfinished">Tipo 3</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="157"/>
        <source>Type 1</source>
        <translation type="unfinished">Tipo 1</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="205"/>
        <source>z = fold2 - fabs( fabs(z + fold) - fold2) - fabs(fold)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="231"/>
        <source>Type 4</source>
        <translation type="unfinished">Tipo 4</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="238"/>
        <source>Type 2</source>
        <translation type="unfinished">Tipo 2</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="245"/>
        <source>z = fabs(z + fold)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="274"/>
        <source>Type 5 Fold2 value:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Type 5 Fold2 Value</source>
        <translation type="obsolete">Tipo 5 Valore di Fold</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="322"/>
        <source>Type 5</source>
        <translation type="unfinished">Tipo 5</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_surf_fold_multi.ui" line="334"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>transf_zVector_axis_swap</name>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="88"/>
        <source>swaps order each iteration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="105"/>
        <source>function order of z.x, z.y and z.z:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="115"/>
        <source>reverse axis sign each iteration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="169"/>
        <source>enable 45 degree rotation of XY plane, for every iteration from start to stop</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="178"/>
        <source>Stop at iteration:</source>
        <translation type="unfinished">Ferma all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="185"/>
        <source>Start at iteration:</source>
        <translation type="unfinished">Inizia all&apos;iterazione:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_transf_zVector_axis_swap.ui" line="235"/>
        <source>Info&amp;rmation</source>
        <translation type="unfinished">Info&amp;rmazione</translation>
    </message>
</context>
<context>
    <name>xenodreambuie</name>
    <message>
        <location filename="../qt_data/fractal_xenodreambuie.ui" line="40"/>
        <source>Alpha angle offset:</source>
        <translation type="unfinished">Offset angolo Alfa:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_xenodreambuie.ui" line="175"/>
        <source>Beta angle offset:</source>
        <translation type="unfinished">Offset angolo Beta:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_xenodreambuie.ui" line="182"/>
        <source>Power:</source>
        <translation type="unfinished">Potenza:</translation>
    </message>
    <message>
        <location filename="../qt_data/fractal_xenodreambuie.ui" line="216"/>
        <source>&amp;Information</source>
        <translation type="unfinished">&amp;Informazioni</translation>
    </message>
</context>
</TS>
