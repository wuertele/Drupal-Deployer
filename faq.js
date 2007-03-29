if (Drupal.jsEnabled) {
  $(document).ready(function () {
    // hide/show answer to question
    $('dd.faq_answer').hide();
    $("dt.faq_question").click(function() {
      $(this).next("dd.faq_answer").toggle();
    });


    // hide/show q/a for a category
    $('div.faq_qa_hide').hide();
    $(".faq_qa_header").click(function() {
      $(this).next("div.faq_qa_hide").toggle();
    });



    // handle faq_category_settings_form
    faq_display_handler();
    questions_top_handler();
    categories_handler();
    $("input[@name=display]").bind("click", faq_display_handler);
    $("input[@name=category_display]").bind("click", categories_handler);
    $("input[@name=hide_sub_categories]").bind("click", sub_cats_handler);


  });
}

function faq_display_handler(event) {
  if ($("input[@name=display]:checked").val() == "questions_inline"
    || $("input[@name=display]:checked").val() == "questions_top") {
    $("input[@name=use_teaser]").removeAttr("disabled");
    $("input[@name=more_link]").removeAttr("disabled");
    $("input[@name=back_to_top]").removeAttr("disabled");
  }
  else {
    $("input[@name=use_teaser]").attr("disabled", "disabled");
    $("input[@name=more_link]").attr("disabled", "disabled");
    $("input[@name=back_to_top]").attr("disabled", "disabled");
  }
}

function questions_top_handler(event) {
  $("input[@name=faq_display]").val() == "questions_top" ?
    $("input[@name=group_questions_top]").removeAttr("disabled"):
    $("input[@name=group_questions_top]").attr("disabled", "disabled");

  $("input[@name=faq_display]").val() == "questions_top" ?
    $("input[@name=answer_category_name]").removeAttr("disabled"):
    $("input[@name=answer_category_name]").attr("disabled", "disabled");
}

function categories_handler(event) {
  if ($("input[@name=faq_display]").val() == "questions_top") {
    $("input[@name=category_display]:checked").val() == "categories_inline" ?
      $("input[@name=group_questions_top]").removeAttr("disabled"):
      $("input[@name=group_questions_top]").attr("disabled", "disabled");
    $("input[@name=category_display]:checked").val() == "new_page" ?
      $("input[@name=answer_category_name]").attr("disabled", "disabled"):
      $("input[@name=answer_category_name]").removeAttr("disabled");
  }
  else {
    $("input[@name=group_questions_top]").attr("disabled", "disabled");
  }

  $("input[@name=category_display]:checked").val() == "categories_inline" ?
    $("input[@name=hide_sub_categories]").attr("disabled", "disabled"):
    $("input[@name=hide_sub_categories]").removeAttr("disabled");
  $("input[@name=category_display]:checked").val() == "categories_inline" ?
    $("input[@name=show_cat_sub_cats]").attr("disabled", "disabled"):
    $("input[@name=show_cat_sub_cats]").removeAttr("disabled");

		sub_cats_handler();
}

function sub_cats_handler(event) {
  if ($("input[@name=hide_sub_categories]:checked").val() == 1) {
    $("input[@name=show_cat_sub_cats]").attr("disabled", "disabled");
		}
		else if ($("input[@name=category_display]:checked").val() != "categories_inline") {
    $("input[@name=show_cat_sub_cats]").removeAttr("disabled");
		}
}
