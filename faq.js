if (Drupal.jsEnabled) {
  $(document).ready(function () {
    $('dd.faq_answer').hide();
    $("dt.faq_question").click(function() {
      $(this).next("dd.faq_answer").toggle();
    });


    $('div.faq_qa_hide').hide();
    $(".faq_qa_header").click(function() {
      $(this).next("div.faq_qa_hide").toggle();
    });
  });
}

