<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the faq module.
 */

/**
 * Generates the settings form for the FAQ module.
 *
 * @param $op
 *   Default value is NULL; determines what are the permissions of the current
 *   user on the FAQ.
 * @return
 *   The output, which contains the HTML code for the settings form generated by
 *   drupal_get_form() function.
 */
function faq_settings_page($op = NULL) {

  $output = drupal_get_form('faq_general_settings_form');

  return $output;
}

/**
 * Define a form to edit the page header and descriptive text.
 *
 * @return
 *   The general settings form code stored in the $form variable, before
 *   converted to HTML.
 */
function faq_general_settings_form() {
  $form['faq_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => variable_get('faq_title', 'Frequently Asked Questions'),
  );

  $form['body_filter']['faq_description'] = array(
    '#type' => 'textarea',
    '#title' => t('FAQ Description'),
    '#default_value' => variable_get('faq_description', ''),
    '#description' => t('Your FAQ description.  This will be placed at the top of the page, above the questions and can serve as an introductory text.'),
    '#rows' => 5,
  );
  $form['body_filter']['faq_description_format'] = filter_form(variable_get('faq_description_format', ''), NULL, array('faq_description_format'));

  return system_settings_form($form);
}

/**
 * Define the elements for the FAQ Settings page - Questions tab.
 *
 * @return
 *   The form code inside the $form array.
 */
function faq_questions_settings_form() {

  drupal_add_js(array('faq' => array('faq_hide_qa_accordion' => variable_get('faq_hide_qa_accordion', FALSE))), 'setting');
  drupal_add_js(array('faq' => array('faq_category_hide_qa_accordion' => variable_get('faq_category_hide_qa_accordion', FALSE))), 'setting');
  drupal_add_js(drupal_get_path('module', 'faq') .'/faq.js', 'module');

  $display_options['questions_inline'] = t('Questions inline');
  $display_options['questions_top'] = t('Clicking on question takes user to answer further down the page');
  $display_options['hide_answer'] = t('Clicking on question opens/hides answer under question');
  $display_options['new_page'] = t('Clicking on question opens the answer in a new page');

  $form['faq_display'] = array(
    '#type' => 'radios',
    '#options' => $display_options,
    '#title' => t('Page layout'),
    '#description' => t('This controls how the questions and answers are displayed on the page and what happens when someone clicks on the question.'),
    '#default_value' => variable_get('faq_display', 'questions_top'),
  );

  $form['faq_questions_misc'] = array(
    '#type' => 'fieldset',
    '#title' => t('Miscellaneous layout settings'),
    '#collapsible' => TRUE,
  );

  $form['faq_questions_misc']['faq_question_listing'] = array(
    '#type' => 'select',
    '#options' => array('ol' => t('Ordered list'), 'ul' => t('Unordered list')),
    '#title' => t('Questions listing style'),
    '#description' => t("This allows to select how the questions listing is presented.  It only applies to the layouts: 'Clicking on question takes user to answer further down the page' and 'Clicking on question opens the answer in a new page'.  An ordered listing would number the questions, whereas an unordered list will have a bullet to the left of each question."),
    '#default_value' => variable_get('faq_question_listing', 'ul'),
  );

  $form['faq_questions_misc']['faq_qa_mark'] = array(
    '#type' => 'checkbox',
    '#title' => t('Label questions and answers'),
    '#description' => t('This option is only valid for the "Questions Inline" layout.  It labels all questions on the faq page with the "question label" setting and all answers with the "answer label" setting.  For example these could be set to "Q:" and "A:".'),
    '#default_value' => variable_get('faq_qa_mark', FALSE),
  );

  $form['faq_questions_misc']['faq_question_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Question Label'),
    '#description' => t('The label to pre-pend to the question text in the "Questions Inline" layout if labelling is enabled.'),
    '#default_value' => variable_get('faq_question_label', 'Q:'),
  );

  $form['faq_questions_misc']['faq_answer_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Answer Label'),
    '#description' => t('The label to pre-pend to the answer text in the "Questions Inline" layout if labelling is enabled.'),
    '#default_value' => variable_get('faq_answer_label', 'A:'),
  );

  $form['faq_questions_misc']['faq_question_length'] = array(
    '#type' => 'radios',
    '#title' => t('Question length'),
    '#options' => array('long' => t('Display longer text'), 'short' => t('Display short text'), 'both' => t('Display both short and long questions')),
    '#description' => t("The length of question text to display on the FAQ page.  The short question will always be displayed in the FAQ blocks."),
    '#default_value' => variable_get('faq_question_length', 'short'),
  );

  $form['faq_questions_misc']['faq_hide_qa_accordion'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use accordion effect for "opens/hides answer under question" layout'),
    '#description' => t('This enables an "accordion" style effect where when a question is clicked, the answer appears beneath, and is then hidden when another question is opened.'),
    '#default_value' => variable_get('faq_hide_qa_accordion', FALSE),
  );

  $form['faq_questions_misc']['faq_show_expand_all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show "expand / collapse all" links for collapsed questions'),
    '#description' => t('The links will only be displayed if using the "opens/hides answer under question" or "opens/hides questions and answers under category" layouts.'),
    '#default_value' => variable_get('faq_show_expand_all', FALSE),
  );

  $form['faq_questions_misc']['faq_use_teaser'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use answer teaser'),
    '#description' => t("This enables the display of the answer teaser text instead of the full answer when using the 'Questions inline' or 'Clicking on question takes user to answer further down the page' display options.  This is useful when you have long descriptive text.  The user can see the full answer by clicking on the question."),
    '#default_value' => variable_get('faq_use_teaser', FALSE),
  );

  $form['faq_questions_misc']['faq_show_node_links'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show node links'),
    '#description' => t('This enables the display of links under the answer text on the faq page.  Examples of these links include "Read more", "Add comment".'),
    '#default_value' => variable_get('faq_show_node_links', FALSE),
  );

  $form['faq_questions_misc']['faq_back_to_top'] = array(
    '#type' => 'textfield',
    '#title' => t('"Back to Top" link text'),
    '#description' => t('This allows the user to change the text displayed for the links which return the user to the top of the page on certain page layouts.  Defaults to "Back to Top".  Leave blank to have no link.'),
    '#default_value' => variable_get('faq_back_to_top', 'Back to Top'),
  );

  $form['faq_questions_misc']['faq_disable_node_links'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable question links to nodes'),
    '#description' => t('This allows the user to prevent the questions being links to the faq node in all layouts except "Clicking on question opens the answer in a new page".'),
    '#default_value' => variable_get('faq_disable_node_links', FALSE),
  );

  $form['faq_questions_misc']['faq_default_sorting'] = array(
    '#type' => 'select',
    '#title' => t('Default sorting for unordered FAQs'),
    '#options' => array('DESC' => t('Date Descending'), 'ASC' => t('Date Ascending')),
    '#description' => t("This controls the default ordering behaviour for new FAQ nodes which haven't been assigned a position."),
    '#default_value' => variable_get('faq_default_sorting', 'DESC'),
  );

  return system_settings_form($form);
}

/**
 * Define the elements for the FAQ Settings page - categories tab.
 *
 * @return
 *   The form code inside the $form array.
 */
function faq_categories_settings_form() {
  if (!module_exists("taxonomy")) {
    drupal_set_message(t('Categorization of questions will not work without the "taxonomy" module being enabled.'), 'error');
  }
  drupal_add_js(array('faq' => array('faq_hide_qa_accordion' => variable_get('faq_hide_qa_accordion', FALSE))), 'setting');
  drupal_add_js(array('faq' => array('faq_category_hide_qa_accordion' => variable_get('faq_category_hide_qa_accordion', FALSE))), 'setting');
  drupal_add_js(drupal_get_path('module', 'faq') .'/faq.js', 'module');

  // Set up a hidden variable.
  $form['faq_display'] = array(
    '#type' => 'hidden',
    '#value' => variable_get('faq_display', 'questions_top'),
  );

  $form['faq_use_categories'] = array(
    '#type' => 'checkbox',
    '#title' => t('Categorize questions'),
    '#description' => t('This allows the user to display the questions according to the categories configured on the add/edit FAQ page.  Use of sub-categories is only recommended for large lists of questions.  The Taxonomy module must be enabled.'),
    '#default_value' => variable_get('faq_use_categories', FALSE),
  );

  $category_options['none'] = t("Don't display");
  $category_options['categories_inline'] = t('Categories inline');
  $category_options['hide_qa'] = t('Clicking on category opens/hides questions and answers under category');
  $category_options['new_page'] = t('Clicking on category opens the questions/answers in a new page');

  $form['faq_category_display'] = array(
    '#type' => 'radios',
    '#options' => $category_options,
    '#title' => t('Categories layout'),
    '#description' => t('This controls how the categories are displayed on the page and what happens when someone clicks on the category.'),
    '#default_value' => variable_get('faq_category_display', 'categories_inline'),
  );

  $form['faq_category_misc'] = array(
    '#type' => 'fieldset',
    '#title' => t('Miscellaneous layout settings'),
    '#collapsible' => TRUE,
  );

  $form['faq_category_misc']['faq_category_listing'] = array(
    '#type' => 'select',
    '#options' => array('ol' => t('Ordered list'), 'ul' => t('Unordered list')),
    '#title' => t('Categories listing style'),
    '#description' => t("This allows to select how the categories listing is presented.  It only applies to the 'Clicking on category opens the questions/answers in a new page' layout.  An ordered listing would number the categories, whereas an unordered list will have a bullet to the left of each category."),
    '#default_value' => variable_get('faq_category_listing', 'ul'),
  );

  $form['faq_category_misc']['faq_category_hide_qa_accordion'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use accordion effect for "opens/hides questions and answers under category" layout'),
    '#description' => t('This enables an "accordion" style effect where when a category is clicked, the questions appears beneath, and is then hidden when another category is opened.'),
    '#default_value' => variable_get('faq_category_hide_qa_accordion', FALSE),
  );

  $form['faq_category_misc']['faq_count'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show FAQ count'),
    '#description' => t('This displays the number of questions in a category after the category name.'),
    '#default_value' => variable_get('faq_count', FALSE),
  );

  $form['faq_category_misc']['faq_answer_category_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display category name for answers'),
    '#description' => t("This allows the user to toggle the visibility of the category name above each answer section for the 'Clicking on question takes user to answer further down the page' question/answer display."),
    '#default_value' => variable_get('faq_answer_category_name', FALSE),
  );

  $form['faq_category_misc']['faq_group_questions_top'] = array(
    '#type' => 'checkbox',
    '#title' => t("Group questions and answers for 'Categories inline'"),
    '#description' => t("This controls how categories are implemented with the 'Clicking on question takes user to answer further down the page' question/answer display."),
    '#default_value' => variable_get('faq_group_questions_top', FALSE),
  );

  $form['faq_category_misc']['faq_hide_child_terms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only show sub-categories when parent category is selected'),
    '#description' => t("This allows the user more control over how and when sub-categories are displayed.  It does not affect the 'Categories inline' display."),
    '#default_value' => variable_get('faq_hide_child_terms', FALSE),
  );

  $form['faq_category_misc']['faq_show_term_page_children'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show sub-categories on FAQ category pages'),
    '#description' => t("Sub-categories with 'faq' nodes will be displayed on the per category FAQ page.  This will also happen if 'Only show sub-categories when parent category is selected' is set."),
    '#default_value' => variable_get('faq_show_term_page_children', FALSE),
  );

  if (module_exists('taxonomy')) {
    $form['faq_category_advanced'] = array(
      '#type' => 'fieldset',
      '#title' => t('Advanced category settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $vocab_options = array();
    $vocabularies = taxonomy_get_vocabularies('faq');
    foreach ($vocabularies as $vid => $vobj) {
      $vocab_options[$vid] = $vobj->name;
    }
    if (!empty($vocab_options)) {
      $form['faq_category_advanced']['faq_omit_vocabulary'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Omit vocabulary'),
        '#description' => t('Terms from these vocabularies will be <em>excluded</em> from the FAQ pages.'),
        '#default_value' => variable_get('faq_omit_vocabulary', 0),
        '#options' => $vocab_options,
        '#multiple' => TRUE,
      );
    }

    $form['faq_category_advanced']['faq_enable_term_links'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable FAQ taxonomy links'),
      '#description' => t('Node links to taxonomy terms will be re-written to point at the FAQ pages instead.'),
      '#default_value' => variable_get('faq_enable_term_links', 1),
    );
  }

  return system_settings_form($form);
}

/**
 * Define the elements for the FAQ Settings page - order tab.
 *
 * @param $form_state
 *   Store the submitted form values.
 * @return
 *   The form code, before being converted to HTML format.
 */
function faq_order_settings_form($form_state, $category = NULL) {
  $order = $date_order = '';

  drupal_add_js(array('faq' => array('faq_hide_qa_accordion' => variable_get('faq_hide_qa_accordion', FALSE))), 'setting');
  drupal_add_js(array('faq' => array('faq_category_hide_qa_accordion' => variable_get('faq_category_hide_qa_accordion', FALSE))), 'setting');
  drupal_add_js(drupal_get_path('module', 'faq') .'/faq.js', 'module');
  drupal_add_css(drupal_get_path('module', 'faq') .'/faq.css');

  $use_categories = variable_get('faq_use_categories', FALSE);
  if (!$use_categories) {
    $step = "order";
  }
  elseif (!isset($form_state['values']) && empty($category)) {
    $step = "categories";
  }
  else {
    $step = "order";
  }
  $form['step'] = array(
    '#type' => 'value',
    '#value' => $step,
  );

  // Categorized q/a.
  if ($step == "categories") {

    // Get list of categories.
    $vocabularies = taxonomy_get_vocabularies('faq');
    $options = array();
    foreach ($vocabularies as $vid => $vobj) {
      $tree = taxonomy_get_tree($vid);
      foreach ($tree as $term) {
        if (!taxonomy_term_count_nodes($term->tid, 'faq')) {
          continue;
        }
        $options[$term->tid] = faq_tt("taxonomy:term:$term->tid:name", $term->name);
        $form['choose_cat']['faq_category'] = array(
          '#type' => 'select',
          '#title' => t('Choose a category'),
          '#description' => t('Choose a category that you wish to order the questions for.'),
          '#options' => $options,
          '#multiple' => FALSE,
        );

        $form['choose_cat']['search'] = array(
          '#type' => 'submit',
          '#value' => t('Search'),
          '#submit' => array('faq_order_settings_choose_cat_form_submit'),
        );
      }
    }

  }

  else {
    $default_sorting = variable_get('faq_default_sorting', 'DESC');
    $default_weight = 0;
    if ($default_sorting != 'DESC') {
      $default_weight = 1000000;
    }

    $options = array();
    if (!empty($form_state['values']['faq_category'])) {
      $category = $form_state['values']['faq_category'];
    }

    // Uncategorized ordering.
    if (empty($category)) {
      $category = 0;
      // Descending.
      if ($default_sorting == 'DESC') {
        $result = db_query(db_rewrite_sql("SELECT n.nid, n.title, if((w.weight IS NULL), %d, w.weight) as weight, n.sticky, n.created FROM {node} n LEFT JOIN {faq_weights} w ON n.nid = w.nid AND w.tid = '%d' WHERE n.type='faq' AND n.status = 1 ORDER BY weight ASC, n.sticky DESC, n.created DESC", "n", "nid"), $default_weight, $category);
      }
      // Ascending.
      else {
        $result = db_query(db_rewrite_sql("SELECT n.nid, n.title, if((w.weight IS NULL), %d, w.weight) as weight, n.sticky, n.created FROM {node} n LEFT JOIN {faq_weights} w ON n.nid = w.nid AND w.tid = '%d' WHERE n.type='faq' AND n.status = 1 ORDER BY weight ASC, n.sticky DESC, n.created ASC", "n", "nid"), $default_weight, $category);
      }
    }
    // Categorized ordering.
    else {
      // Descending.
      if ($default_sorting == 'DESC') {
        $result = db_query(db_rewrite_sql("SELECT n.nid, n.title, if((w.weight IS NULL), %d, w.weight) as weight, n.sticky, n.created FROM {node} n INNER JOIN {term_node} tn ON (n.nid = tn.nid AND n.vid = tn.vid) LEFT JOIN {faq_weights} w ON n.nid = w.nid AND w.tid = '%d' WHERE n.type='faq' AND n.status = 1 AND tn.tid = '%d' ORDER BY weight ASC, n.sticky DESC, n.created DESC", "n", "nid"), $default_weight, $category, $category);
      }
      // Ascending.
      else {
        $result = db_query(db_rewrite_sql("SELECT n.nid, n.title, if((w.weight IS NULL), %d, w.weight) as weight, n.sticky, n.created FROM {node} n INNER JOIN {term_node} tn ON (n.nid = tn.nid AND n.vid = tn.vid) LEFT JOIN {faq_weights} w ON n.nid = w.nid AND w.tid = '%d' WHERE n.type='faq' AND n.status = 1 AND tn.tid = '%d' ORDER BY weight ASC, n.sticky DESC, n.created ASC", "n", "nid"), $default_weight, $category, $category);
      }
    }


    while ($node = db_fetch_object($result)) {
      $options[$node->nid] = $node->title;
    }

    $form['weight']['faq_category'] = array(
      '#type' => 'value',
      '#value' => $category,
    );

    // Show table ordering form.
    $form['order_no_cats']['#tree'] = TRUE;
    $form['order_no_cats']['#theme'] = 'faq_draggable_question_order_table';

    $i = 0;
    foreach ($options as $nid => $title) {
      $form['order_no_cats'][$i]['nid'] = array('#type' => 'hidden', '#value' => $nid);
      $form['order_no_cats'][$i]['title'] = array('#value' => $title);
      $form['order_no_cats'][$i]['sort'] = array('#type' => 'weight', '#delta' => count($options), '#default_value' => $i);
      $i++;
    }

    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save order'),
      '#weight' => 3,
      '#attributes' => array('onclick' => 'faq_update_order();'),
      '#submit' => array('faq_order_settings_reorder_form_submit'),
    );
  }


  return $form;
}

/**
 * Function set the rebuild of the form in the FAQ Settings - Weight tab.
 *
 * @param $form
 *   Array, containing the form structure.
 * @param &$form_state
 *   The 'rebuild' key inside $form_state['rebuild'] structure, overrides the
 *   'redirect' key: when it is set to TRUE, the form will be rebuilt from
 *   scratch and displayed on screen.
 */
function faq_order_settings_choose_cat_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Save the options set by the user in the FAQ Settings - Weight tab.
 *
 * @param $form
 *   Array, containing the form structure.
 * @param &$form_state
 *   $form_state['values'] stores the submitted values from the form.
 */
function faq_order_settings_reorder_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Save order') && !empty($form_state['values']['order_no_cats'])) {

    foreach ($form_state['values']['order_no_cats'] as $i => $faq) {
      $nid = $faq['nid'];
      $index = $faq['sort'];
      $result = db_query("DELETE FROM {faq_weights} WHERE tid = %d AND nid = %d", $form_state['values']['faq_category'], $nid);
      $result = db_query("INSERT INTO {faq_weights} (tid, nid, weight) VALUES(%d, %d, %d)", $form_state['values']['faq_category'], $nid, $index);

      // If node translation module enabled, update order of the translation
      // node counterparts.
      if (module_exists('translation')) {
        $node = node_load($nid);
        if ($node->tnid) {
          $translations = translation_node_get_translations($node->tnid);
          if (!empty($translations)) {
            foreach ($translations as $language => $tnode) {
              $result = db_query("DELETE FROM {faq_weights} WHERE tid = %d AND nid = %d", $form_state['values']['faq_category'], $tnode->nid);
              $result = db_query("INSERT INTO {faq_weights} (tid, nid, weight) VALUES(%d, %d, %d)", $form_state['values']['faq_category'], $tnode->nid, $index);
            }
          }
        }
      }
    }

    drupal_set_message(t('Configuration has been updated.'));
  }
}
